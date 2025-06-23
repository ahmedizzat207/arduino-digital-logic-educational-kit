# Arduino Digital Logic Educational Kit

## Introduction

Welcome! This project is designed for anyone interested in learning the basics of digital logic and Arduino programming. No prior experience is required. By building and using this kit, you'll gain hands-on experience with binary numbers, logic gates, and analog-to-digital conversion, all while learning how to wire up and program an Arduino.

## What is This Kit?

The Arduino Digital Logic Educational Kit is an interactive learning tool. It lets you:

- Enter binary numbers using switches and see their decimal/hex equivalents.
- Experiment with logic gates (AND, OR, XOR, XNOR).
- Learn how analog signals are converted to digital values.
- Test your knowledge with quizzes.

## What's Included?

- Arduino code (`src/GroupProject.ino`)
- Example wiring diagrams (see below)
- This README and a beginner-friendly guide

## Hardware Requirements

- 1 x Arduino Board (Uno, Nano, or compatible)
- 1 x 16x2 LCD Screen (I2C interface recommended)
- 8 x Pushbutton Switches (for binary input)
- 8 x LEDs (for output display)
- 1 x 74HC595 Shift Register (to control LEDs)
- 1 x Pushbutton (for mode selection)
- 1 x Potentiometer (for analog input)
- 1 x Analog Sensor (optional, e.g., light sensor)
- Jumper wires, breadboard, resistors

## Circuit Diagram

> **Tip:** If you're new to breadboarding, search for "Arduino 74HC595 LED wiring" and "Arduino I2C LCD wiring" for visual guides.

```
[Arduino] --(I2C)--> [LCD]
[Arduino Pins 2-9] --> [Switches]
[Arduino Pins 10,11,12] --> [74HC595] --> [LEDs]
[Pin 13] --> [Mode Button]
[A1] --> [Potentiometer]
[A3] --> [Analog Sensor]
```

## Software Setup

1. **Install Arduino IDE:** Download from [arduino.cc](https://www.arduino.cc/en/software)
2. **Install Libraries:**
   - For I2C LCD: Install `LiquidCrystal_I2C` via Library Manager.
3. **Open the Code:**
   - Open `src/GroupProject.ino` in Arduino IDE.
4. **Select Board & Port:**
   - Go to Tools > Board > Select your Arduino model.
   - Go to Tools > Port > Select the correct port.
5. **Upload:**
   - Click the Upload button (right arrow icon).

## How to Use

- **Switch Modes:** Press the mode button to cycle through modes. The LCD will show which mode is active.
- **Manual Mode:**
  - Use the 8 switches to enter a binary number.
  - The LCD shows the decimal and hexadecimal values.
- **Quiz Mode:**
  - The LCD displays a decimal number.
  - Set the switches to match its binary value, then press the mode button to check your answer.
- **Logic Gate Mode:**
  - Use switches to set two 4-bit binary numbers (A and B).
  - The LCD shows the result of AND, OR, XOR, XNOR operations.
- **ADC Mode:**
  - Turn the potentiometer or use the analog sensor.
  - The LCD shows the analog value and its digital conversion.

## Example: Entering a Binary Number

1. Set switches to 00001111 (first 4 off, last 4 on).
2. In Manual Mode, the LCD will show:
   - Binary: 00001111
   - Decimal: 15
   - Hex: 0xF

## Troubleshooting

- **LCD Not Displaying:**
  - Check I2C connections and address (may need to scan for correct address).
- **LEDs Not Lighting:**
  - Double-check shift register wiring and code pin assignments.
- **Upload Fails:**
  - Ensure correct board/port is selected and no other program is using the port.

## Frequently Asked Questions

**Q: Can I use a different Arduino model?**
A: Yes, as long as it has enough pins.

**Q: Do I need all 8 switches/LEDs?**
A: You can use fewer, but update the code accordingly.

**Q: Where can I get help?**
A: See the Resources section below or open an issue on GitHub.

## Resources

- [Arduino Official Documentation](https://www.arduino.cc/en/Guide)
- [74HC595 Shift Register Tutorial](https://www.arduino.cc/en/Tutorial/Foundations/ShiftOut)
- [I2C LCD Guide](https://lastminuteengineers.com/i2c-lcd-arduino-tutorial/)
- [Binary Numbers for Beginners](https://www.mathsisfun.com/binary-number-system.html)

## Contributing

Contributions are welcome! To contribute:

1. Fork this repository.
2. Create a new branch for your feature or fix.
3. Commit your changes with clear messages.
4. Open a pull request describing your changes.

## License

This project is licensed under the MIT License. See the LICENSE file for details.
