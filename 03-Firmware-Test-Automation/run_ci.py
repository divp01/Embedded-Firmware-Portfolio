"""
CI/CD Validation Runner & KPI Dashboard Generator.

Usage:
  python run_ci.py           # Runs full matrix (prod gates + fault demos) & builds dashboard
  python run_ci.py --strict  # Fails exit code if ANY scenario fails (PR regression gate mode)
"""

import argparse  # Standard library module to parse command-line flags like --strict.
import json      # Standard library module to read test_matrix.json into Python dicts.
from pathlib import Path  # Object-oriented filesystem paths (works cleanly on Windows & Linux).
import sys       # Provides sys.exit(...) so CI pipelines receive pass (0) or fail (1) codes.

# Import stimulus generator and simulated HAL transport from our framework package.
from framework.device_hal import SimulatedXRTransport, generate_stimulus_frames
# Import KPI dataclass, evaluator function, and JSON/HTML report writer.
from framework.kpi_and_triage import KPIReport, evaluate_kpis, write_reports

ROOT_DIR = Path(__file__).resolve().parent        # Absolute path to the project root folder.
CONFIG_PATH = ROOT_DIR / "config" / "test_matrix.json"  # Path to declarative KPI budgets & scenarios.
REPORTS_DIR = ROOT_DIR / "reports"                # Folder where JSON & HTML reports will be saved.


def run_matrix(strict: bool = False) -> int:
    """Execute every scenario in `test_matrix.json`, print summary, and write reports."""
    # Read test_matrix.json text and parse it into a Python dictionary.
    matrix = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
    budgets = matrix["kpi_budgets"]      # Extract pass/fail thresholds (latency, power, drops, etc.).
    scenarios = matrix["scenarios"]      # Extract dictionary of test scenarios to execute.

    reports = []                         # List to collect one KPIReport per scenario.
    for name, cfg in scenarios.items():  # Loop over every scenario name and its config dict.
        # Build a deterministic stream of synthetic IMU frames for this scenario.
        frames = generate_stimulus_frames(
            imu_rate_hz=cfg["imu_rate_hz"],        # Sensor sampling rate in Hz (e.g., 200 or 400).
            duration_sec=cfg["duration_sec"],      # Total simulated capture time in seconds.
            gesture_events=cfg["gesture_events"],  # Number of true head-nod gestures to inject.
        )
        # Create a HAL transport pointing to our simulated XR low-power subsystem.
        transport = SimulatedXRTransport(fault_mode=cfg["fault_mode"])
        # Push the frames through the SUT and capture timestamps, power, and trace logs.
        telemetry = transport.execute_scenario(
            frames=frames, imu_rate_hz=cfg["imu_rate_hz"]
        )
        # Compute KPIs, check pass/fail against budgets, and run automated root-cause triage.
        reports.append(evaluate_kpis(name, telemetry, budgets))

    write_reports(reports, REPORTS_DIR)  # Save reports/kpi_summary.json and reports/kpi_dashboard.html.
    _print_console_summary(reports)      # Print a human-readable summary table to the terminal.

    # Decide which failures should fail the CI exit code:
    # - In normal mode, ignore `fault_demo_*` scenarios (they exist to demonstrate triage).
    # - In --strict mode, treat every failed scenario as a blocking CI failure.
    blocking_failures = [
        r for r in reports
        if not r.passed and (strict or not r.scenario_name.startswith("fault_demo_"))
    ]
    return 1 if blocking_failures else 0  # Return 1 (fail) if any blocking failure exists, else 0 (pass).


def _print_console_summary(reports: list[KPIReport]) -> None:
    """Print an easy-to-read CI summary table to stdout."""
    print("\n=== XR LPAI Validation Summary ===")  # Header banner for terminal output.
    for r in reports:                               # Iterate through each scenario's KPIReport.
        status = "PASS" if r.passed else "FAIL"     # Convert boolean pass/fail into readable text.
        # Print aligned metrics: scenario name, P95 latency, average power, drop %, detection %.
        print(
            f"[{status}] {r.scenario_name:26s} | "
            f"P95={r.p95_latency_ms:5.1f}ms | Power={r.avg_power_mw:5.2f}mW | "
            f"Drop={r.drop_rate_pct:4.2f}% | Det={r.detection_rate_pct:5.1f}%"
        )
        for cause in r.root_causes:                 # If the scenario failed, print diagnosed causes.
            print(f"       -> Triage: {cause}")
    print(f"\nArtifacts generated in: {REPORTS_DIR}")  # Tell the user where reports were saved.


if __name__ == "__main__":  # Only run CLI parsing when executed directly (`python run_ci.py`).
    # Create an argument parser for command-line flags.
    parser = argparse.ArgumentParser(description="XR LPAI CI Validation Runner")
    # Register the optional `--strict` boolean flag (defaults to False).
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Return non-zero exit code if any scenario (including fault demos) fails.",
    )
    args = parser.parse_args()                  # Parse flags passed by the user/CI system.
    sys.exit(run_matrix(strict=args.strict))    # Run validation and exit with code 0 or 1.
