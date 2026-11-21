# XR Low-Power AI (LPAI) System Validation Project

A beginner-friendly, zero-dependency Python project designed to teach **System Test Architecture** for Qualcomm's **XR System Test Engineer** role.

## Architecture

- [`config/test_matrix.json`](config/test_matrix.json) — Declarative KPI budgets (P95 latency, average power in mW, frame drop rate, false wakes) and scenario matrix.
- [`sut/xr_low_power_subsystem.py`](sut/xr_low_power_subsystem.py) — Simulated always-on XR Sensor Hub + LPAI DSP + Power State Machine (`DEEP_SLEEP`, `LPAI_LISTEN`, `AP_WAKE`) with fault injection.
- [`framework/device_hal.py`](framework/device_hal.py) — Device Abstraction Layer (`XRDeviceTransport`) decoupling test logic from target transport (`SimulatedXRTransport` vs `ADBXRTransport`).
- [`framework/kpi_and_triage.py`](framework/kpi_and_triage.py) — KPI calculator, automated trace-log root-cause analyzer, and JSON/HTML dashboard generator.
- [`tests/test_xr_lpai_validation.py`](tests/test_xr_lpai_validation.py) — Automated test suite covering functional, 400 Hz stress, false-wake power penalty, and fault-triage verification.
- [`run_ci.py`](run_ci.py) — CI/CD runner generating `reports/kpi_summary.json` and `reports/kpi_dashboard.html`.

## How to Run

```powershell
# 1. Run automated validation tests
python -m unittest discover -s tests -v

# 2. Run CI matrix and generate KPI dashboard + JSON report
python run_ci.py

# 3. Strict CI PR-gate mode (exits with code 1 if any scenario fails)
python run_ci.py --strict
```

