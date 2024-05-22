#Description
This is a project exploring the usage of Arduino board RP2040 Connect in entertainment as a motion controller for gaming purpose (3d spaceship obstacle dodging) by combining voice recognition.

##Demo
![image](https://github.com/KaiWong05452/IMU-game-using-Arduino-RP2040-Connect/blob/master/Demo.png)

##Project objectives
1. IMU acceleration and gyroscope data mapped to Euler angles.
2. Transfer data from Arduino BLE Peripheral to Laptop as BLE Central.
3. Transfer data from Laptop to Unity game engine through Socket connection.
4. Spaceship game basic design.
5. Real-time voice recognition to detect user command for game control.
6. Transfer recognized user command to Unity game engine through Socket connection.

##Design
![image](https://github.com/KaiWong05452/IMU-game-using-Arduino-RP2040-Connect/blob/master/Design.png)

#IMU algorithm
The solution from aster94 to convert IMU data to euler angles using Mahony algorithm, refer to https://github.com/aster94/SensorFusion

##Voice recognition
voice recogintion used the RealtimeSTT from KoljaB, refer to https://github.com/KoljaB/RealtimeSTT

##Unity assests
The assests used in unity are totally free and those can be found in the unity assest store.
