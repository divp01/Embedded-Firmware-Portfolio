"""
Automated System Test Suite for XR Low-Power AI (LPAI) Validation.

NOTE FOR INTERVIEWS:
This uses Python's built-in `unittest` so it runs with zero pip installs.
In a Qualcomm CI environment, the exact same structure maps 1-to-1 to `pytest`:
  - `setUpClass` -> `@pytest.fixture(scope="module")`
  - `_run_scenario(...)` -> `@pytest.mark.parametrize("scenario", ...)`
  - `self.assertTrue(...)` -> `assert ...`
"""

import json                # Reads our JSON test matrix config.
from pathlib import Path   # Builds an OS-independent path to config/test_matrix.json.
import unittest            # Built-in Python test runner & assertion library.

# Import our HAL stimulus generator, simulator transport, and KPI evaluator.
from framework.device_hal import SimulatedXRTransport, generate_stimulus_frames
from framework.kpi_and_triage import KPIReport, evaluate_kpis

# Resolve path: tests/test_xr_lpai_validation.py -> parent(tests) -> parent(root)/config/test_matrix.json.
CONFIG_PATH = Path(__file__).resolve().parents[1] / "config" / "test_matrix.json"


class TestXRLPAIValidation(unittest.TestCase):
    """End-to-end validation covering Functional, Stress, Power & Triage."""

    @classmethod  # Runs once before any test method in this class executes.
    def setUpClass(cls) -> None:
        # Load declarative KPI budgets & scenario parameters from JSON.
        cls.matrix = json.loads(CONFIG_PATH.read_text(encoding="utf-8"))
        cls.budgets = cls.matrix["kpi_budgets"]   # Shared thresholds (e.g., max P95 latency = 50ms).
        cls.scenarios = cls.matrix["scenarios"]   # Named scenarios (smoke, stress, fault demos).

    def _run_scenario(
        self, scenario_name: str, inject_false_noise: bool = False
    ) -> KPIReport:
        """Shared helper: build stimulus -> run via HAL -> evaluate KPIs."""
        cfg = self.scenarios[scenario_name]       # Look up scenario config dict by name.
        frames = generate_stimulus_frames(        # Build synthetic IMU frames for this scenario.
            imu_rate_hz=cfg["imu_rate_hz"],
            duration_sec=cfg["duration_sec"],
            gesture_events=cfg["gesture_events"],
            inject_false_noise=inject_false_noise,
        )
        # Instantiate HAL backend (swap SimulatedXRTransport -> ADBXRTransport for real boards).
        transport = SimulatedXRTransport(fault_mode=cfg["fault_mode"])
        telemetry = transport.execute_scenario(   # Execute scenario on target and gather telemetry.
            frames=frames, imu_rate_hz=cfg["imu_rate_hz"]
        )
        return evaluate_kpis(                     # Compute KPIs, check budgets, and run triage.
            scenario_name=scenario_name, telemetry=telemetry, budgets=self.budgets
        )

    def test_functional_smoke_passes(self) -> None:
        """1. Functional + KPI gate: valid gestures wake AP within latency & power budgets."""
        report = self._run_scenario("functional_smoke")
        self.assertTrue(report.passed, msg=f"Unexpected failures: {report.failures}")
        self.assertEqual(report.detection_rate_pct, 100.0)  # Every injected gesture must wake AP.
        self.assertEqual(report.false_wakes, 0)             # Zero spurious wakes allowed.

    def test_high_rate_stress_passes(self) -> None:
        """2. Stress test: 400Hz IMU stream must have 0% frame drops and low avg power."""
        report = self._run_scenario("stress_high_rate")
        self.assertTrue(report.passed, msg=f"Stress failures: {report.failures}")
        self.assertEqual(report.drop_rate_pct, 0.0)         # Sensor driver must keep up at 400Hz.
        self.assertLessEqual(report.avg_power_mw, self.budgets["max_avg_power_mw"])

    def test_false_wake_is_caught_and_triaged(self) -> None:
        """3. Power/Accuracy regression: noisy false AP wake must fail gate & auto-triage."""
        report = self._run_scenario("functional_smoke", inject_false_noise=True)
        self.assertFalse(report.passed)                     # Gate must fail when a false wake occurs.
        self.assertEqual(report.false_wakes, 1)             # Verify the 1 noisy frame woke the AP.
        self.assertTrue(                                    # Verify triage pinpointed FALSE_WAKE_TRIGGER.
            any("FALSE_WAKE_TRIGGER" in cause for cause in report.root_causes),
            msg=f"Expected FALSE_WAKE_TRIGGER diagnosis, got: {report.root_causes}",
        )

    def test_dsp_throttle_fault_is_caught_and_triaged(self) -> None:
        """4. Fault-injection test: DSP throttling breaks P95 latency and logs root cause."""
        report = self._run_scenario("fault_demo_dsp_throttle")
        self.assertFalse(report.passed)                     # Gate must catch latency regression.
        self.assertGreater(report.p95_latency_ms, self.budgets["max_p95_latency_ms"])
        self.assertTrue(                                    # Verify triage pinpointed DSP_CLOCK_THROTTLE.
            any("DSP_CLOCK_THROTTLE" in cause for cause in report.root_causes),
            msg=f"Expected DSP_CLOCK_THROTTLE diagnosis, got: {report.root_causes}",
        )

    def test_fifo_overflow_fault_is_caught_and_triaged(self) -> None:
        """5. Fault-injection test: 400Hz IMU FIFO overflow breaks drop-rate KPI & auto-triages."""
        report = self._run_scenario("fault_demo_fifo_overflow")
        self.assertFalse(report.passed)                     # Gate must catch dropped sensor frames.
        self.assertGreater(report.drop_rate_pct, self.budgets["max_drop_rate_pct"])
        self.assertTrue(                                    # Verify triage pinpointed FIFO_OVERFLOW.
            any("FIFO_OVERFLOW" in cause for cause in report.root_causes),
            msg=f"Expected FIFO_OVERFLOW diagnosis, got: {report.root_causes}",
        )


if __name__ == "__main__":  # Allows running this test file directly with `python tests/test_xr_lpai_validation.py`.
    unittest.main()
