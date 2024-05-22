import serial
import numpy as np
import soundfile as sf

# Open the serial port
ser = serial.Serial('COM21', 9600)

# Buffer to store PCM data
pcm_data = []

while True:
    if ser.in_waiting > 0:
        # Read a line from the serial port
        line = ser.readline().decode('utf-8').strip()
        # Convert the line to an integer and add it to the buffer
        pcm_data.append(int(line))

        # If enough data has been collected
        if len(pcm_data) >= 256000:
            # Convert the list to a NumPy array
            pcm_data = np.array(pcm_data, dtype=np.int16)

            # Normalize PCM data to -1 to 1 range
            pcm_data = pcm_data / np.max(np.abs(pcm_data))

            # Save as a WAV file
            sf.write('output.wav', pcm_data, 16000)
            print("output")
            # Clear the PCM data buffer
            pcm_data = []