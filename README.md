# Description
This is a project exploring the usage of Arduino board RP2040 Connect in entertainment as a motion controller for gaming purpose (3d spaceship obstacle dodging) by combining voice recognition.

## Demo
![image](https://github.com/KaiWong05452/IMU-game-using-Arduino-RP2040-Connect/blob/master/Demo.png)




https://github.com/KaiWong05452/IMU-game-using-Arduino-RP2040-Connect/assets/127105744/480a82a8-e507-4853-9734-17a04c833f82




## Project objectives
1. IMU acceleration and gyroscope data mapped to Euler angles.
2. Transfer data from Arduino BLE Peripheral to Laptop as BLE Central.
3. Transfer data from Laptop to Unity game engine through Socket connection.
4. Spaceship game basic design.
5. Real-time voice recognition to detect user command for game control.
6. Transfer the recognized user command to Unity game engine through Socket connection.

## Design
![image](https://github.com/KaiWong05452/IMU-game-using-Arduino-RP2040-Connect/blob/master/Design.png)

## IMU algorithm
The solution from aster94 to convert IMU data to Euler angles using the Mahony algorithm, refer to [GitHub Pages](https://github.com/aster94/SensorFusion).

## Voice recognition
voice recognition used the RealtimeSTT from KoljaB, refer to [GitHub Pages](https://github.com/KoljaB/RealtimeSTT).

## Unity assets
The assets used in unity are free and those can be found in the unity asset store.
