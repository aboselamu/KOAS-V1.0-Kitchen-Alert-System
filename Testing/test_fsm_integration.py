import serial
import time
import pytest

# ==============================================================================
# CONFIGURATION
# Ensure NUCLEO_COM_PORT matches your Device Manager
# ==============================================================================
NUCLEO_COM_PORT = 'COM9' 
BAUD_RATE = 115200

def get_serial_connection():
    return serial.Serial(NUCLEO_COM_PORT, BAUD_RATE, timeout=2)

def test_ticket_insertion_alarm():
    """Test 1: Verify that breaking the IR beam triggers the buzzer."""
    print("\n--- [Test 1] Starting Ticket Insertion Test ---")
    ser = get_serial_connection()
    time.sleep(2)
    
    try:
        # Reset state
        ser.write(b'R')
        time.sleep(0.5)

        # Trigger Alarm
        print("    [Action] Breaking beam ('B')...")
        ser.write(b'B')
        time.sleep(0.5)

        # Query status
        ser.write(b'Q')
        response = ser.readline().decode('utf-8').strip()
        
        assert response == '1', f"FAIL: Expected '1', got '{response}'"
        print("    ✅ SUCCESS: Alarm Triggered.")
        
        # Cleanup
        ser.write(b'R')
    finally:
        ser.close()

def test_human_silence_button():
    """Test 2: Verify that the physical button silences the alarm."""
    print("\n--- [Test 2] Starting Human Silence Test ---")
    ser = get_serial_connection()
    time.sleep(2)
    
    try:
        # 1. Force Alarm
        ser.write(b'B')
        time.sleep(0.5)
        
        # 2. Simulate Human Button Press
        print("    [Action] Pressing physical silence button ('S')...")
        ser.write(b'S')
        time.sleep(0.5)
        
        # 3. Check if Buzzer is OFF
        ser.write(b'Q')
        response = ser.readline().decode('utf-8').strip()
        
        assert response == '0', f"FAIL: Expected '0' (Silenced), got '{response}'"
        print("    ✅ SUCCESS: Buzzer silenced by button.")
        
        # 4. Final Cleanup
        ser.write(b'R')
    finally:
        ser.close()

if __name__ == "__main__":
    # If run directly, run all tests
    pytest.main([__file__, "-s"])