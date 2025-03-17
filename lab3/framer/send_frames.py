from time import sleep
import numpy as np
import serial

ser = serial.serial_for_url('rfc2217://localhost:4000', baudrate=115200)
frames = 6955

for x in range(232, frames):
    with open(f"frames/frame_{x:06}.bin", "rb") as file:
        data = bytearray(file.read())
        data = np.array(data).reshape(16, 20)
        result = []
        # Process the first 4 characters (from the first line)
        for i in range(4):
            # Extract a 5x10 slice for the current character
            char_array = data[0:8, i*5:(i+1)*5]
            result.append(char_array)

        # Process the remaining 4 characters (from the second line)
        for i in range(4):
            # Extract a 5x10 slice for the current character
            char_array = data[8:16, i*5:(i+1)*5]
            result.append(char_array)
        for array in result:
            char = bytearray([0] * 8)
            for row in range(8):
                for bit in range(5):
                    char[row] = (char[row] << 1) | (array[row][bit] & 1)
            ser.write(char)
        sleep(1 / 10)
        print("Message received!")
        # exit()
