"""
Simulated XR Low-Power Subsystem (System Under Test / SUT).

INTERVIEW CONTEXT (Qualcomm XR / LPAI):
On XR glasses or mobile platforms, running sensors + AI continuously on the main
Application Processor (AP / CPU) destroys battery life (~300-500 mW).
Instead, an always-on Low-Power AI (LPAI) island (e.g., Hexagon DSP + Sensor Hub)
monitors IMU/Audio at ~4-15 mW and wakes the AP ONLY when a real gesture or
wake-word occurs.

This module simulates that hardware/firmware pipeline so we can practice
system validation, telemetry collection, and root-cause debugging without a
physical Qualcomm board on our desk.
"""

from dataclasses import dataclass, field  # Helps define clean data containers without boilerplate __init__.
from enum import Enum                     # Lets us define fixed named states for power modes.
import random                             # Used with a fixed seed so simulated jitter is reproducible.
from typing import List                   # Type hint for lists (e.g., List[SensorFrame]).


class PowerState(str, Enum):
    """Power states of the XR subsystem (from lowest to highest power draw)."""

    DEEP_SLEEP = "DEEP_SLEEP"    # Sensors idle / minimal retention (~4 mW).
    LPAI_LISTEN = "LPAI_LISTEN"  # DSP running always-on AI inference (~11.5 mW).
    AP_WAKE = "AP_WAKE"          # Main CPU/Display woken up (~340 mW) — very expensive!


# Lookup table mapping each PowerState to its typical power consumption in milliwatts (mW).
POWER_DRAW_MW = {
    PowerState.DEEP_SLEEP: 4.0,     # Lowest power floor.
    PowerState.LPAI_LISTEN: 11.5,   # Always-on DSP + sensor hub budget.
    PowerState.AP_WAKE: 340.0,      # High spike when main AP wakes up.
}


@dataclass
class SensorFrame:
    """Represents one time-synchronized IMU + Audio frame pushed to the DSP."""

    seq_id: int                    # Monotonic packet counter (used to detect dropped frames).
    timestamp_ms: float            # Hardware timestamp in milliseconds when sample was captured.
    accel_z_g: float               # Z-axis accelerometer reading in g (~1.0g at rest, ~2.4g on nod).
    is_gesture_event: bool = False # Ground truth flag: True if this frame contains a real gesture.
    is_false_noise: bool = False   # True if frame contains misleading noise that shouldn't wake AP.


@dataclass
class InferenceEvent:
    """Output emitted by the LPAI DSP when a gesture/wake-word triggers."""

    seq_id: int                 # Which sensor packet triggered this wake event.
    sensor_timestamp_ms: float  # When the sensor captured the gesture.
    wake_timestamp_ms: float    # When the AP received the wake interrupt.
    e2e_latency_ms: float       # End-to-end latency = wake_timestamp_ms - sensor_timestamp_ms.
    expected_gesture: bool      # True = valid detection; False = false positive wake!


@dataclass
class TelemetryBundle:
    """
    Raw telemetry captured during a test run.
    In real Qualcomm/Android testing, this comes from Perfetto/ftrace, logcat,
    DSP QXDM/diag logs, and a Monsoon/NI power monitor.
    """

    total_frames_sent: int = 0     # Total stimulus frames pushed into the subsystem.
    frames_processed: int = 0      # Frames successfully read by the DSP (excludes drops).
    frames_dropped: int = 0        # Frames lost due to FIFO overflow or bus contention.
    expected_gestures: int = 0     # Ground-truth count of real gestures in the stimulus.
    true_wakes: int = 0            # Count of real gestures correctly detected by LPAI.
    false_wakes: int = 0           # Count of accidental AP wakes caused by noise (power killer!).
    inference_events: List[InferenceEvent] = field(default_factory=list)  # All wake events + latencies.
    power_samples_mw: List[float] = field(default_factory=list)           # Per-tick power readings (mW).
    trace_logs: List[str] = field(default_factory=list)                   # Structured kernel/DSP log lines.


class XRLowPowerSubsystem:
    """
    Simulates an always-on XR Sensor -> DSP LPAI -> AP Wake pipeline.

    Supports fault injection (`fault_mode`) so our test framework can practice
    catching and automatically root-causing realistic firmware/hardware bugs.
    """

    def __init__(self, fault_mode: str = "none", seed: int = 42) -> None:
        self.fault_mode = fault_mode          # Fault scenario: "none", "fifo_overflow", "dsp_throttle", etc.
        self._rng = random.Random(seed)       # Seeded RNG ensures identical results across re-runs.
        self._state = PowerState.LPAI_LISTEN  # Subsystem boots into always-on listening mode.

    def run_stream(self, frames: List[SensorFrame], imu_rate_hz: int) -> TelemetryBundle:
        """
        Process a stream of sensor frames and return synchronized telemetry.

        Key validation concepts modeled here:
        1. IMU FIFO buffering & overflow drops at high ODR (Output Data Rate).
        2. DSP inference latency (plus thermal clock-throttling jitter).
        3. Power-state transitions (staying in LPAI_LISTEN vs waking the AP).
        """
        telemetry = TelemetryBundle(total_frames_sent=len(frames))  # Initialize telemetry container.
        telemetry.trace_logs.append(                                # Log boot banner into trace buffer.
            f"INFO [SYS] Boot LPAI subsystem rate={imu_rate_hz}Hz fault_mode={self.fault_mode}"
        )

        for frame in frames:                    # Process each incoming sensor sample sequentially.
            if frame.is_gesture_event:          # Track ground-truth gesture count for accuracy math.
                telemetry.expected_gestures += 1

            # 1. Check whether the IMU FIFO overflows at high sampling rate.
            if self._should_drop_frame(frame.seq_id, imu_rate_hz):
                telemetry.frames_dropped += 1   # Increment dropped-packet counter.
                telemetry.trace_logs.append(    # Emit driver error signature for root-cause triage.
                    f"ERR [IMU_DRV] FIFO_OVERFLOW seq={frame.seq_id} rate={imu_rate_hz}Hz dropped=1"
                )
                # Record baseline listening power for this tick and skip processing the lost sample.
                telemetry.power_samples_mw.append(POWER_DRAW_MW[PowerState.LPAI_LISTEN])
                continue

            telemetry.frames_processed += 1     # Frame survived FIFO; count it as processed.

            # 2. Run LPAI detection logic for real gestures or noisy false triggers.
            if frame.is_gesture_event:
                self._handle_wake_trigger(frame, expected=True, telemetry=telemetry)
            elif self._should_false_wake(frame):
                self._handle_wake_trigger(frame, expected=False, telemetry=telemetry)
            else:
                # Normal idle frame: stay on low-power DSP, never wake the main AP.
                self._state = PowerState.LPAI_LISTEN
                # Add small realistic sensor/PMIC measurement ripple (+/- 0.4 mW).
                noisy_power = POWER_DRAW_MW[self._state] + self._rng.uniform(-0.4, 0.4)
                telemetry.power_samples_mw.append(round(noisy_power, 2))

        telemetry.trace_logs.append(            # Emit final summary log line when stream ends.
            f"INFO [SYS] Stream complete processed={telemetry.frames_processed} "
            f"dropped={telemetry.frames_dropped}"
        )
        return telemetry                        # Return all collected metrics & logs to the HAL.

    def _should_drop_frame(self, seq_id: int, imu_rate_hz: int) -> bool:
        """Inject deterministic IMU FIFO drops under `fifo_overflow` fault mode."""
        if self.fault_mode == "fifo_overflow" and imu_rate_hz >= 400:  # Only overflows at >= 400Hz.
            return seq_id % 25 == 0                                    # Drop 1 out of every 25 frames (4%).
        return False                                                   # Otherwise no frames are dropped.

    def _should_false_wake(self, frame: SensorFrame) -> bool:
        """Simulate an uncalibrated model waking the AP on background noise."""
        if self.fault_mode == "false_wake_noise" and frame.seq_id % 40 == 0:  # Periodic noise spike.
            return True
        return frame.is_false_noise  # Or explicitly injected false-noise frame from test stimulus.

    def _handle_wake_trigger(
        self, frame: SensorFrame, expected: bool, telemetry: TelemetryBundle
    ) -> None:
        """Compute DSP-to-AP wake latency, transition power state, and log traces."""
        # Normal DSP inference takes ~18-28 ms; thermal throttling degrades it to 62-88 ms.
        if self.fault_mode == "dsp_throttle":
            latency_ms = round(self._rng.uniform(62.0, 88.0), 2)   # Exceeds 50ms latency budget.
            telemetry.trace_logs.append(                           # Log DSP clock throttling warning.
                f"WARN [LPAI_DSP] DSP_CLOCK_THROTTLE freq_mhz=120 seq={frame.seq_id} "
                f"latency_ms={latency_ms}"
            )
        else:
            latency_ms = round(self._rng.uniform(18.0, 28.0), 2)   # Healthy DSP latency within budget.

        self._state = PowerState.AP_WAKE                           # Transition hardware to AP_WAKE.
        telemetry.power_samples_mw.append(POWER_DRAW_MW[PowerState.AP_WAKE])  # Record 340mW spike.

        if expected:
            telemetry.true_wakes += 1                              # Count valid wake detection.
            telemetry.trace_logs.append(                           # Log successful wake event.
                f"INFO [LPAI_AI] TRUE_WAKE seq={frame.seq_id} latency_ms={latency_ms}"
            )
        else:
            telemetry.false_wakes += 1                             # Count false wake penalty.
            telemetry.trace_logs.append(                           # Log false wake signature for triage.
                f"WARN [LPAI_AI] FALSE_WAKE_TRIGGER seq={frame.seq_id} power_mw=340.0"
            )

        telemetry.inference_events.append(                         # Store timing record for P50/P95/P99 math.
            InferenceEvent(
                seq_id=frame.seq_id,
                sensor_timestamp_ms=frame.timestamp_ms,
                wake_timestamp_ms=round(frame.timestamp_ms + latency_ms, 2),
                e2e_latency_ms=latency_ms,
                expected_gesture=expected,
            )
        )
        # Immediately return subsystem to always-on listen after servicing the interrupt.
        self._state = PowerState.LPAI_LISTEN
