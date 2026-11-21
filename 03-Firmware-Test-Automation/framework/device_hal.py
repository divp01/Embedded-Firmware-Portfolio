"""
Device Abstraction Layer (HAL) & Stimulus Generator.

INTERVIEW CONTEXT:
A core principle of System Test Architecture is separating WHAT you test (test
cases & KPI assertions) from HOW you talk to the device (transport).
Because all tests talk to `XRDeviceTransport`, the exact same test matrix can run:
  1. In Software-in-the-Loop (SIL) mode on a developer laptop (`SimulatedXRTransport`)
  2. In Hardware-in-the-Loop (HIL) mode on a Qualcomm XR board (`ADBXRTransport`)
"""

from abc import ABC, abstractmethod  # Standard library support for Abstract Base Classes (interfaces).
from typing import List              # Type hint for lists of SensorFrame objects.

# Import the simulated SUT and its data classes.
from sut.xr_low_power_subsystem import (
    SensorFrame,
    TelemetryBundle,
    XRLowPowerSubsystem,
)


def generate_stimulus_frames(
    imu_rate_hz: int,
    duration_sec: float,
    gesture_events: int,
    inject_false_noise: bool = False,
) -> List[SensorFrame]:
    """
    Generate deterministic synthetic 6-axis IMU frames for reproducible testing.

    In a physical lab (HIL), this is replaced by a robotic motion rig or
    pre-recorded sensor playback injected into the HAL.
    """
    total_samples = max(1, int(imu_rate_hz * duration_sec))  # Total number of IMU samples in window.
    dt_ms = 1000.0 / imu_rate_hz                             # Time interval between consecutive samples (ms).

    gesture_indices = set()                                  # Set of frame indices that represent gestures.
    if gesture_events > 0:
        step = max(1, total_samples // (gesture_events + 1)) # Evenly space gestures across the duration.
        for i in range(1, gesture_events + 1):
            idx = min(total_samples - 1, i * step)           # Clamp index within valid range [0..N-1].
            gesture_indices.add(idx)

    frames: List[SensorFrame] = []                           # Output list of synthetic SensorFrames.
    for seq in range(total_samples):                         # Build each frame one by one.
        is_gesture = seq in gesture_indices                  # True if this sample is a ground-truth gesture.
        accel_z = 2.4 if is_gesture else 1.0                 # 1.0g static gravity vs 2.4g head-nod peak.
        # Optionally inject one noisy non-gesture spike right in the middle of the capture.
        false_noise = inject_false_noise and (not is_gesture) and (seq == total_samples // 2)
        frames.append(
            SensorFrame(
                seq_id=seq,                                  # Monotonic sequence ID for drop detection.
                timestamp_ms=round(seq * dt_ms, 2),          # Synthetic sensor hardware timestamp.
                accel_z_g=accel_z,                           # Z-axis acceleration value.
                is_gesture_event=is_gesture,                 # Ground-truth label.
                is_false_noise=false_noise,                  # False-trigger noise label.
            )
        )
    return frames                                            # Return complete stimulus stream.


class XRDeviceTransport(ABC):
    """Abstract interface every XR target backend (Sim, ADB, SSH, Rig) must implement."""

    @abstractmethod  # Forces subclasses to implement `execute_scenario` or raise TypeError.
    def execute_scenario(
        self, frames: List[SensorFrame], imu_rate_hz: int
    ) -> TelemetryBundle:
        """Send stimulus frames to the target and collect synchronized telemetry."""


class SimulatedXRTransport(XRDeviceTransport):
    """Software-in-the-Loop (SIL) backend for fast CI/CD and local debugging."""

    def __init__(self, fault_mode: str = "none", seed: int = 42) -> None:
        # Instantiate our local Python XR subsystem simulator with optional fault injection.
        self._sut = XRLowPowerSubsystem(fault_mode=fault_mode, seed=seed)

    def execute_scenario(
        self, frames: List[SensorFrame], imu_rate_hz: int
    ) -> TelemetryBundle:
        # Forward frames to the local simulator and return its TelemetryBundle.
        return self._sut.run_stream(frames=frames, imu_rate_hz=imu_rate_hz)


class ADBXRTransport(XRDeviceTransport):
    """
    Hardware-in-the-Loop (HIL) skeleton for real Android / Qualcomm XR targets.

    Shows interviewers how you extend this exact framework to real hardware:
      - `adb push` playback dataset or trigger robotic motion rig
      - `adb shell am broadcast ...` to start LPAI test harness on Hexagon DSP
      - `adb logcat -d` + `perfetto` trace pull + Monsoon power monitor sync
    """

    def __init__(self, device_serial: str) -> None:
        self.device_serial = device_serial  # Android `adb devices` serial ID (e.g., "12ab34cd").

    def execute_scenario(
        self, frames: List[SensorFrame], imu_rate_hz: int
    ) -> TelemetryBundle:
        # Placeholder guard so tests fail clearly if someone selects HIL without hardware attached.
        raise NotImplementedError(
            f"ADBXRTransport({self.device_serial}) requires a connected XR device. "
            "Use SimulatedXRTransport for local SIL runs."
        )
