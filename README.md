# auto-v5.0
This project is the design and programming of an RF transmitter and receiver to control a toy car. <br>

The test_car and test_control folders have the last working version of code in Arduino (Arduino Nano in transmitter and receptor). The RF modules are NRF24. <br>

The control has an I2C OLED Display to present the information about analog measures and commands to send to the car. These commands are: velocity to rear motor (back and forward), the direction to front motor, the state of lights, the ON/OFF engine and an extra analog channel to control a servomotor to angle of a FPV camera. <br>

The car has the ability to communicate with the control and send the status of connection and its level of battery (in millivolts). These information is presenten in real-time in the OLED Display on control (with the control battery level too).
