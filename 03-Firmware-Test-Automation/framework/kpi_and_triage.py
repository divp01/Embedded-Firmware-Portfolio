"""
KPI Engine, Automated Root-Cause Triage & Reporting Dashboard.

INTERVIEW CONTEXT:
Good test automation doesn't just say "FAIL". On complex XR systems (Audio,
IMU, Camera, DSP, Kernel), engineers lose hours digging through 50 MB of logs.
This module demonstrates:
  1. Computing system KPIs (P50/P95/P99 latency, mean power mW, drop rate, accuracy).
  2. Gating pass/fail against quantitative budgets.
  3. Automated log/trace correlation to pinpoint the probable root cause.
  4. Generating CI-ready JSON artifacts and an HTML KPI dashboard.
"""

from dataclasses import asdict, dataclass  # `asdict` converts KPIReport into JSON-friendly dict.
import html                                # Escapes log strings safely before rendering into HTML.
import json                                # Writes machine-readable kpi_summary.json artifacts.
import math                                # Provides `ceil` for nearest-rank percentile indexing.
from pathlib import Path                   # Cross-platform file path handling.
from typing import Dict, List              # Type hints for clarity and static checks.

from sut.xr_low_power_subsystem import TelemetryBundle  # Input telemetry structure from the HAL.


@dataclass
class KPIReport:
    """Aggregated validation metrics and triage findings for a single scenario."""

    scenario_name: str       # Name of the test scenario (e.g., "functional_smoke").
    passed: bool             # True only if every KPI satisfies its configured budget.
    p50_latency_ms: float    # Median end-to-end sensor-to-wake latency.
    p95_latency_ms: float    # 95th percentile latency (catches tail jitter / throttling).
    p99_latency_ms: float    # 99th percentile worst-case tail latency.
    avg_power_mw: float      # Mean subsystem power draw across the whole scenario (mW).
    drop_rate_pct: float     # Percentage of sensor packets dropped before reaching LPAI.
    detection_rate_pct: float  # Percentage of expected gestures successfully detected.
    false_wakes: int         # Total unwanted AP wakes caused by noise.
    failures: List[str]      # Human-readable list of every KPI threshold that failed.
    root_causes: List[str]   # Automated log-correlated root-cause explanations.


def compute_percentile(values: List[float], pct: float) -> float:
    """Compute percentile (0-100) without external dependencies like NumPy."""
    if not values:           # Guard against empty latency lists (e.g., 0 triggers).
        return 0.0
    ordered = sorted(values) # Sort latencies ascending so index corresponds to rank.
    # Nearest-rank index formula clamped between 0 and len(ordered) - 1.
    rank = max(0, min(len(ordered) - 1, math.ceil((pct / 100.0) * len(ordered)) - 1))
    return round(ordered[rank], 2)


# Signature table mapping trace log keywords -> actionable engineering diagnosis.
ROOT_CAUSE_RULES = [
    (
        "FIFO_OVERFLOW",
        "IMU Driver / Sensor Hub FIFO overflow detected at high ODR; "
        "increase FIFO watermark or drain interval.",
    ),
    (
        "DSP_CLOCK_THROTTLE",
        "Hexagon/LPAI DSP clock throttled (thermal or power governor); "
        "check DSP OPP vote and thermal mitigation policy.",
    ),
    (
        "FALSE_WAKE_TRIGGER",
        "False AP wake triggered by noise; recalibrate LPAI confidence threshold "
        "or front-end bandpass filter.",
    ),
]


def analyze_root_cause(trace_logs: List[str], failures: List[str]) -> List[str]:
    """
    Correlate KPI failures with structured trace/log signatures.

    If a KPI failed but no known signature matched, explicitly flag it for
    manual `perfetto` / `logcat` inspection.
    """
    if not failures:         # If every KPI passed, no root-cause diagnosis is needed.
        return []

    findings: List[str] = []
    for signature, explanation in ROOT_CAUSE_RULES:  # Scan logs for each known failure signature.
        matching_lines = [line for line in trace_logs if signature in line]
        if matching_lines:
            # Report signature, hit count, first sample log line, and suggested owner/fix.
            findings.append(
                f"[{signature}] ({len(matching_lines)}x, e.g. '{matching_lines[0]}') -> {explanation}"
            )

    if not findings:         # Fallback if a regression occurred without any known error signature.
        findings.append(
            "[UNCLASSIFIED] KPI failed without a known log signature; "
            "attach full Perfetto/logcat bundle for manual triage."
        )
    return findings


def evaluate_kpis(
    scenario_name: str, telemetry: TelemetryBundle, budgets: Dict[str, float]
) -> KPIReport:
    """Compute all KPIs from telemetry, check budgets, and run root-cause triage."""
    # Extract all recorded end-to-end latencies from inference events.
    latencies = [ev.e2e_latency_ms for ev in telemetry.inference_events]
    p50 = compute_percentile(latencies, 50)  # Compute 50th percentile (median).
    p95 = compute_percentile(latencies, 95)  # Compute 95th percentile.
    p99 = compute_percentile(latencies, 99)  # Compute 99th percentile.

    # Compute mean power draw (mW), guarding against empty power sample lists.
    avg_power = (
        round(sum(telemetry.power_samples_mw) / len(telemetry.power_samples_mw), 2)
        if telemetry.power_samples_mw
        else 0.0
    )
    # Compute frame drop percentage = (dropped / sent) * 100.
    drop_rate = (
        round((telemetry.frames_dropped / telemetry.total_frames_sent) * 100.0, 2)
        if telemetry.total_frames_sent
        else 0.0
    )
    # Compute gesture detection rate = (true_wakes / expected_gestures) * 100.
    detection_rate = (
        round((telemetry.true_wakes / telemetry.expected_gestures) * 100.0, 2)
        if telemetry.expected_gestures
        else 100.0
    )

    failures: List[str] = []  # Collect every KPI threshold breach.
    if p95 > budgets["max_p95_latency_ms"]:
        failures.append(
            f"P95 latency {p95} ms > budget {budgets['max_p95_latency_ms']} ms"
        )
    if avg_power > budgets["max_avg_power_mw"]:
        failures.append(
            f"Avg power {avg_power} mW > budget {budgets['max_avg_power_mw']} mW"
        )
    if drop_rate > budgets["max_drop_rate_pct"]:
        failures.append(
            f"Frame drop rate {drop_rate}% > budget {budgets['max_drop_rate_pct']}%"
        )
    if telemetry.false_wakes > budgets["max_false_wakes"]:
        failures.append(
            f"False wakes {telemetry.false_wakes} > budget {budgets['max_false_wakes']}"
        )
    if detection_rate < budgets["min_detection_rate_pct"]:
        failures.append(
            f"Detection rate {detection_rate}% < budget {budgets['min_detection_rate_pct']}%"
        )

    # Correlate any failures with captured trace logs.
    root_causes = analyze_root_cause(telemetry.trace_logs, failures)
    return KPIReport(
        scenario_name=scenario_name,
        passed=len(failures) == 0,  # Scenario passes iff zero KPI breaches occurred.
        p50_latency_ms=p50,
        p95_latency_ms=p95,
        p99_latency_ms=p99,
        avg_power_mw=avg_power,
        drop_rate_pct=drop_rate,
        detection_rate_pct=detection_rate,
        false_wakes=telemetry.false_wakes,
        failures=failures,
        root_causes=root_causes,
    )


def write_reports(reports: List[KPIReport], output_dir: Path) -> None:
    """Persist machine-readable JSON KPIs and a human-readable HTML dashboard."""
    output_dir.mkdir(parents=True, exist_ok=True)  # Ensure `reports/` folder exists.

    # 1. Write machine-readable JSON summary (ideal for historical trend DBs / Grafana).
    json_path = output_dir / "kpi_summary.json"
    json_path.write_text(
        json.dumps([asdict(r) for r in reports], indent=2), encoding="utf-8"
    )

    # 2. Build one HTML table row per scenario.
    rows_html = []
    for r in reports:
        status_badge = (
            '<span class="badge pass">PASS</span>'
            if r.passed
            else '<span class="badge fail">FAIL</span>'
        )
        triage_text = (
            "<br>".join(html.escape(c) for c in r.root_causes)
            if r.root_causes
            else "All KPIs within budget"
        )
        rows_html.append(
            f"<tr>"
            f"<td>{html.escape(r.scenario_name)}</td>"
            f"<td>{status_badge}</td>"
            f"<td>{r.p50_latency_ms} / {r.p95_latency_ms} / {r.p99_latency_ms}</td>"
            f"<td>{r.avg_power_mw}</td>"
            f"<td>{r.drop_rate_pct}%</td>"
            f"<td>{r.detection_rate_pct}%</td>"
            f"<td>{r.false_wakes}</td>"
            f"<td>{triage_text}</td>"
            f"</tr>"
        )

    # 3. Render standalone dark-mode HTML dashboard with inline CSS (no external CDN needed).
    dashboard_html = f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>XR LPAI System Validation KPI Dashboard</title>
  <style>
    body {{ font-family: Segoe UI, Arial, sans-serif; margin: 24px; background: #0f172a; color: #e2e8f0; }}
    h1 {{ color: #38bdf8; margin-bottom: 4px; }}
    p.sub {{ color: #94a3b8; margin-top: 0; }}
    table {{ width: 100%; border-collapse: collapse; margin-top: 16px; background: #1e293b; }}
    th, td {{ border: 1px solid #334155; padding: 10px 12px; text-align: left; font-size: 14px; }}
    th {{ background: #0b1220; color: #cbd5e1; }}
    .badge {{ padding: 3px 8px; border-radius: 4px; font-weight: 700; font-size: 12px; }}
    .pass {{ background: #14532d; color: #86efac; }}
    .fail {{ background: #7f1d1d; color: #fca5a5; }}
  </style>
</head>
<body>
  <h1>XR Low-Power AI (LPAI) Validation Dashboard</h1>
  <p class="sub">Automated Functional, Latency, Power &amp; Root-Cause Triage Report</p>
  <table>
    <thead>
      <tr>
        <th>Scenario</th>
        <th>Status</th>
        <th>Latency P50/P95/P99 (ms)</th>
        <th>Avg Power (mW)</th>
        <th>Drop Rate</th>
        <th>Detection Rate</th>
        <th>False Wakes</th>
        <th>Automated Root-Cause Triage</th>
      </tr>
    </thead>
    <tbody>
      {"".join(rows_html)}
    </tbody>
  </table>
</body>
</html>
"""
    # Save HTML dashboard to `reports/kpi_dashboard.html`.
    (output_dir / "kpi_dashboard.html").write_text(dashboard_html, encoding="utf-8")
