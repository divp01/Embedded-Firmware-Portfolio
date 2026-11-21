"""
SUPER SIMPLE EXAMPLE: Smart Glasses Wake-Up Test (1 file, ~70 lines).

Think of a System Test Engineer's job in 3 simple steps:
  1. Send an input to the device     (Example: user nods their head)
  2. Measure what the device did     (Did it wake up? How fast? How much battery?)
  3. Check PASS or FAIL + read logs  (If it failed, read the log to find out why)
"""


# ==============================================================================
# STEP 1: THE DEVICE (Simulated Smart Glasses Chip / DSP)
# ==============================================================================
def smart_glasses_chip(user_action, has_bug=False):
    """
    Pretend this function is the Qualcomm low-power chip inside smart glasses.
    Input:  user_action -> "quiet", "head_nod", or "bus_noise"
    Output: a dictionary with wake_up (True/False), latency_ms, power_mw, and a log line.
    """
    if user_action == "head_nod":
        # Real gesture! The chip should wake up the screen quickly (~25 ms).
        latency = 85 if has_bug else 25  # If bug is ON, chip is slow (85 ms > 50 ms limit).
        return {
            "woke_up": True,
            "latency_ms": latency,
            "power_mw": 300,  # Waking the main CPU/screen uses high power briefly.
            "log": "DSP: Detected head_nod" + (" [SLOW: CPU throttled!]" if has_bug else ""),
        }

    if user_action == "bus_noise":
        # Background noise! Glasses should IGNORE it to save battery.
        if has_bug:
            # Bug: noise accidentally woke up the main CPU -> wastes battery!
            return {
                "woke_up": True,
                "latency_ms": 30,
                "power_mw": 300,
                "log": "DSP_ERR: False wake triggered by background noise!",
            }
        return {
            "woke_up": False,
            "latency_ms": 0,
            "power_mw": 10,  # Low power listening mode (good for battery!).
            "log": "DSP: Ignored noise, staying in low-power sleep",
        }

    # "quiet" -> nothing happened, stay in low-power sleep (10 mW).
    return {
        "woke_up": False,
        "latency_ms": 0,
        "power_mw": 10,
        "log": "DSP: Quiet, sleeping at 10 mW",
    }


# ==============================================================================
# STEP 2: THE AUTOMATED TESTS (What you build as an XR System Test Engineer)
# ==============================================================================
def run_all_tests(has_bug=False):
    print(f"\n--- Running Tests (has_bug={has_bug}) ---")

    # TEST 1 (Functional + Latency): Head nod must wake glasses in under 50 ms.
    result1 = smart_glasses_chip("head_nod", has_bug=has_bug)
    if result1["woke_up"] and result1["latency_ms"] <= 50:
        print("[PASS] Test 1 (Head Nod): Woke up fast at", result1["latency_ms"], "ms")
    else:
        print("[FAIL] Test 1 (Head Nod): Too slow!", result1["latency_ms"], "ms")
        print("       Debug Log ->", result1["log"])  # Log tells us WHY it failed!

    # TEST 2 (Power / Battery): Background noise must NOT wake glasses (power <= 15 mW).
    result2 = smart_glasses_chip("bus_noise", has_bug=has_bug)
    if (not result2["woke_up"]) and result2["power_mw"] <= 15:
        print("[PASS] Test 2 (Battery): Stayed asleep at", result2["power_mw"], "mW")
    else:
        print("[FAIL] Test 2 (Battery): False wake drained", result2["power_mw"], "mW!")
        print("       Debug Log ->", result2["log"])  # Log tells us WHY it failed!


# ==============================================================================
# STEP 3: RUN IT!
# ==============================================================================
if __name__ == "__main__":
    run_all_tests(has_bug=False)  # Run 1: Healthy firmware -> both tests PASS.
    run_all_tests(has_bug=True)   # Run 2: Buggy firmware   -> tests FAIL and print debug logs.

