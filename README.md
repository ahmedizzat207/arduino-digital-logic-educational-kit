# Arduino Digital Logic Educational Kit

## Overview
The Arduino Digital Logic Educational Kit is an interactive tool designed to help students learn about digital logic concepts, including binary numbers, logic gates, and analog-to-digital conversion. This project provides a hands-on experience through various modes that allow users to engage with the material actively.

## Features
- **Manual Mode**: Input binary numbers using switches and see their decimal and hexadecimal equivalents.
- **Quiz Mode**: Test your knowledge by matching binary representations to randomly generated decimal numbers.
- **Logic Gate Mode**: Experiment with basic logic gates (AND, OR, XOR, XNOR) using binary inputs.
- **ADC Mode**: Visualize how analog signals are converted to digital values in real-time.

## Setup Instructions
1. **Hardware Requirements**:
   - Arduino Board
   - 16x2 LCD Screen (I2C)
   - 8 Switches (for binary input)
   - 8 LEDs (controlled via a shift register)
   - 74HC595 Shift Register
   - Mode Button
   - Potentiometer
   - Analog Sensor

2. **Connections**:
   - Connect the LCD to the Arduino using I2C.
   - Connect the switches to digital pins 2-9.
   - Connect the LEDs through the shift register (pins 10, 11, 12 for data, latch, and clock).
   - Connect the mode button to pin 13 and the potentiometer to analog pin A1.
   - Connect the analog sensor to analog pin A2.

3. **Software Requirements**:
   - Install the Arduino IDE.
   - Ensure the necessary libraries for I2C and LCD control are included in your Arduino IDE.

4. **Uploading the Code**:
   - Open the `src/GroupProject.ino` file in the Arduino IDE.
   - Select the appropriate board and port from the Tools menu.
   - Upload the code to the Arduino.

## Usage
- Press the mode button to switch between different modes.
- Follow the on-screen instructions displayed on the LCD for each mode.
- Use the switches to input binary numbers and interact with the system.

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request for any improvements or bug fixes.

## License
This project is licensed under the MIT License. See the LICENSE file for more details.