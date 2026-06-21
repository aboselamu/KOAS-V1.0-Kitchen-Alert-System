import serial
import time

def test_full_fsm_cycle():
    print("\n--- Starting HIL Integration Test ---")
    ser = serial.Serial('COM9', 115200, timeout=0.5)

    time.sleep(0.5)
    ser.reset_input_buffer()
    ser.reset_output_buffer()

    # 1. ESTABLISH COMMS (PING)
    print("1. Pinging Nucleo...")
    boot_msg = ""
    for _ in range(10):  
        ser.write(b'?')
        boot_msg = ser.readline().decode('utf-8').strip()
        if boot_msg == "Nucleo Ready":
            break
    assert boot_msg == "Nucleo Ready", "Failed to connect to Nucleo."

    try:
        # 2. HARDWARE INITIALIZATION STATE
        print("   -> Initializing hardware lines (Beam Restored, Button Released)...")
        ser.write(b'R') 
        time.sleep(0.1)
        ser.write(b'U') 
        time.sleep(0.5) 

        # 3. TRIGGER THE STATE (BREAK BEAM)
        print("3. Breaking the IR beam ('B')...")
        ser.write(b'B')
        time.sleep(0.2) # Give the state machine a moment to transition to alarm state

        # 4. VERIFY THE STATE (WINDOWED SAMPLING FOR TOGGLING BUZZER)
        print("4. Sampling buzzer state over a 1-second window...")
        buzzer_triggered = False
        
        # Sample 20 times over 1 second (every 50ms) to catch the "ON" phase of the beep
        for sample in range(20):
            ser.reset_input_buffer()
            ser.write(b'Q')
            reply = ser.readline().decode('utf-8').strip()
            
            if reply == '1':
                buzzer_triggered = True
                print(f"   -> Catch! Buzzer detected ON ('1') at sample {sample}")
                break # We caught it! No need to keep sampling.
                
            time.sleep(0.04) 

        assert buzzer_triggered, "FSM FAILED: Buzzer never turned ON during the 1-second observation window."
        print("   -> ASSERTION PASSED! Buzzer pulsing verified.")

    finally:
        # 5. TEARDOWN: Safely restore hardware states
        print("5. Teardown: Restoring beam and clearing alarm...")
        ser.write(b'R') 
        time.sleep(0.2) 
        ser.write(b'S') # Press Silence
        time.sleep(0.5) 
        ser.write(b'U') # Release Silence
        time.sleep(0.2) 

        print("--- Test Complete & State Cleaned Up ---")
        ser.close()