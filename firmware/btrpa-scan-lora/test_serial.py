#!/usr/bin/env python3
"""
Quick serial monitor to verify Heltec device is working
"""
import serial
import time
import sys

try:
    # Open serial port
    ser = serial.Serial('/dev/cu.usbserial-0001', 115200, timeout=1)
    print("Connected to Heltec device on /dev/cu.usbserial-0001")
    print("Reading for 10 seconds...")
    print("=" * 60)

    start_time = time.time()

    while time.time() - start_time < 10:
        if ser.in_waiting > 0:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(line)
            except Exception as e:
                pass

    print("=" * 60)
    print("\nDone! If you see output above, the device is working.")
    ser.close()

except serial.SerialException as e:
    print(f"Error: Could not open serial port: {e}")
    print("Make sure the device is plugged in and no other program is using it.")
    sys.exit(1)
except KeyboardInterrupt:
    print("\nStopped by user")
    ser.close()
