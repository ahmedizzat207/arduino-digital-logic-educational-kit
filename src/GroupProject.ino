/*
 * Multi-Mode Digital Logic Educational Kit
 * =============================================================
 *
 * What This Project Does:
 * ----------------------
 * This is an educational tool that helps students learn about:
 * 1. Binary numbers (0s and 1s)
 * 2. Converting between different number systems (binary, decimal, hexadecimal)
 * 3. Logic gates (AND, OR, XOR, XNOR)
 * 4. Analog to digital conversion
 *
 * How It Works:
 * ------------
 * - Students can press buttons (switches) to input binary numbers
 * - The LCD screen shows different numbers and results
 * - LEDs light up to show the binary input
 * - A button lets you switch between different modes
 *
 * The Four Modes:
 * ----------------------------
 * 1. Manual Mode:
 *    - Press switches to make binary numbers
 *    - See the decimal and hexadecimal equivalents
 *    - Great for learning how binary numbers work!
 *
 * 2. Quiz Mode:
 *    - The system gives you a number
 *    - You try to make that number using the switches
 *    - Perfect for practice and learning!
 *
 * 3. Logic Gate Mode:
 *    - Learn how AND, OR, XOR, and XNOR gates work
 *    - Use switches to input two 4-bit numbers
 *    - Turn the potentiometer to select different gates
 *    - See the result of the logic operation
 *
 * 4. ADC Mode:
 *    - See how analog signals become digital
 *    - Turn the potentiometer to change the analog input
 *    - Watch how the digital value changes
 *
 * Hardware You Need:
 * ----------------
 * 1. Arduino Board: The brain of the project
 * 2. LCD Screen (16x2): Shows numbers and text
 *    - Connected via I2C (a way to send data using just 2 wires)
 * 3. 8 Switches: For inputting binary numbers
 *    - Connected to pins 2-9
 *    - When you press a switch, it inputs a 1 or high signal
 * 4. 8 LEDs: Shows the binary input
 *   - Connected through a shift register (74HC595)
 * 5. Shift Register (74HC595):
 *    - A chip that helps control multiple LEDs with fewer pins
 *   - Uses 3 pins (data, latch, clock) to control 8 LEDs
 * 6. Mode Button: For changing between modes
 *    - Connected to analog pin A0
 * 7. Potentiometer: For adjusting values
 *    - Connected to analog pin A1
 * 8. Analog Sensor: For ADC (Analog to Digital Conversion) mode
 *    - Connected to analog pin A2
 *
 * Pin Connections:
 * --------------
 * Digital Pins:
 * - Pins 2-9: Connected to switches (input)
 * - Pin 10: Data pin for LED shift register
 * - Pin 11: Latch pin for LED shift register
 * - Pin 12: Clock pin for LED shift register
 *
 * Analog Pins:
 * - A0: Mode selection button
 * - A1: Potentiometer for logic gate selection
 * - A2: Analog sensor for ADC mode
 * - A5: Used for random number generation
 */

// === Libraries We Need ===
#include <Wire.h>                   // For I2C communication with LCD
#include <LiquidCrystal_I2C.h>      // For controlling the LCD screen

// === LCD Screen Setup ===
// This is a 16x2 LCD, meaning it can show 16 characters on each of its 2 lines.
// We use I2C to connect it, which simplifies wiring and communication.

// Why I2C?
// - I2C only needs 2 wires to talk to the LCD instead of the typical 6 wires which is the minimum for a standard LCD connection.
// - Those two wires are:
//   1. SDA (Serial Data): Sends the actual information
//   2. SCL (Serial Clock): Keeps everything synchronized
//
// The '0x27' in lcd(0x27) is the I2C address:
// - Think of it like a house number on a street
// - Each I2C device needs a unique address
//
// This creates an 'lcd' object we can use to:
// - Write text and numbers to the display
// - Control the backlight
// - Position the cursor
// - Clear the screen

LiquidCrystal_I2C lcd(0x27, 16, 2); // Create LCD object (object is a c++ class instance) with I2C address 0x27, 16 columns, 2 rows
// Note: The address 0x27 is common for many I2C LCDs, but it can vary.

// === LED Control Pins (Using 74HC595 Shift Register) ===
// A shift register lets us control 8 LEDs using just 3 pins!
// Latch pin (RCLK): Controls when data is loaded to output pins
// Clock pin (SRCLK): Controls data shifting
// Data pin (SER): Serial data input

// Think of it like a helper chip that takes our data and spreads it to the LEDs
const int latchPin = 11; // Pin 11: Tells the helper when to update the LEDs
const int clockPin = 12; // Pin 12: Helps move the data into place
const int dataPin = 10;  // Pin 10: Carries the actual LED data (on/off information)

// === Switch Pins ===
// These are our input switches for binary numbers
// When a switch is pressed (LOW), it represents a 0
// When a switch is not pressed (HIGH), it represents a 1
const int switchPins[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // Connected to digital pins 2-9

// === Control Inputs ===
// These are our special control inputs
const int modeButtonPin = 13;   // Button to change modes (Manual/Quiz/Logic/ADC)
const int potPin = A1;          // Knob (potentiometer) for selecting logic gates
const int analogSensorPin = A2; // Sensor for analog to digital conversion (ADC) mode

// === Mode Tracking ===
// Variables to keep track of which mode we're in
byte currentMode = 0;      // Which mode are we in? (0-3)
const byte totalModes = 4; // How many modes do we have?

// === Button Debounce Variables ===
/*
 * What is Button Debouncing?
 * -------------------------
 * When you press a physical button, it doesn't make a clean on/off signal.
 * Instead, it "bounces" - creating multiple signals over a few milliseconds.
 * Without debouncing, one press might register as multiple presses!
 * To learn more about debouncing, check out this link: https://www.youtube.com/watch?v=jYOYgU2vlSE
 *
 * How These Variables Help:
 * -----------------------
 * lastButtonState:
 *   - Remembers if the button was pressed (LOW) or released (HIGH)
 *   - Used to detect when the button state changes
 *
 * lastDebounce:
 *   - Stores when the last button change happened (in milliseconds)
 *   - Helps us wait before accepting new button presses
 *
 * debounceDelay:
 *   - How long to wait (50ms) before accepting a button press as real
 *   - This delay filters out the "bounce" signals
 */
bool lastButtonState = HIGH;            // Start assuming button is not pressed
unsigned long lastDebounce = 0;         // Time of last button state change
const unsigned long debounceDelay = 50; // Wait 50ms to confirm button press

// === Button State Handling ===
/*
 * How Button Reading Works:
 * -----------------------
 * 1. Read current button state (pressed = LOW, released = HIGH)
 * 2. If state changed from last reading:
 *    - Record the time of change in lastDebounce
 *    - Wait for debounceDelay (50ms) to ensure it's a real press
 * 3. If state stays stable for 50ms:
 *    - Accept it as a real button press/release
 *    - Update lastButtonState for next comparison
 *
 * Example Timeline:
 * ---------------
 * 0ms   : Button pressed → signals bounce between HIGH/LOW
 * 1-49ms: Ignore changes during debounceDelay
 * 50ms  : If still pressed, accept as real button press
 */

// === Quiz Mode Variables ===
// Target value for quiz mode (0-255). Students need to set switches to match this value
byte quizTarget = 0;

// Tracks whether current quiz question is shown on LCD. Prevents question from being
// repeatedly displayed when mode is active
bool questionDisplayed = false;

// === Logic Gate Mode Constants ===
// Array of logic gate names for display on LCD. Corresponds to LogicType enum indices
const char *logicNames[4] = {"AND", "OR", "XOR", "XNOR"};

// Enumeration of supported logic gates. Used to track current gate type and
// calculate correct outputs in logic gate mode
enum LogicType
{
  GATE_AND, // Logical AND:  Output=1 only if both inputs=1
  GATE_OR,  // Logical OR:   Output=1 if either input=1
  GATE_XOR, // Logical XOR:  Output=1 if inputs are different
  GATE_XNOR // Logical XNOR: Output=1 if inputs are same
};
/*
 * Function: setup
 * --------------
 * This is where we prepare everything before our program starts running
 * It runs once when the Arduino is powered on or reset
 *
 * What This Function Does:
 * 1. Sets up the LCD screen
 *    - Tells it it's a 16x2 display (16 characters, 2 lines)
 *    - Turns on the backlight
 *    - Shows "Mode: Manual" to start
 *
 * 2. Sets up all our pins
 *    - Tells Arduino which pins are inputs (switches and button)
 *    - Tells Arduino which pins are outputs (LED control)
 *    - Activates internal pullup resistors (makes switches work properly without external resistors unsing pull-up configuration)
 *
 * 3. Prepares the random number generator for Quiz mode
 *    - Uses electrical noise from analog pin A5 to make random numbers
 */
void setup()
{
  // Initialize LCD with 16 columns and 2 rows
  lcd.init();                // Initialize the LCD
  lcd.backlight(1);           // Turn on the LCD backlight
  lcd.setCursor(0, 0);       // Set cursor to the first line, first column
  lcd.print("Mode: Manual"); // Display initial mode

  // Configure pin modes
  pinMode(modeButtonPin, INPUT_PULLUP); // Mode button with internal pullup resistor
  pinMode(latchPin, OUTPUT);            // 74HC595 latch pin
  pinMode(clockPin, OUTPUT);            // 74HC595 clock pin
  pinMode(dataPin, OUTPUT);             // 74HC595 data pin

  // Configure all switch pins as inputs with pullup resistors
  for (int i = 0; i < 8; i++)
  {
    pinMode(switchPins[i], INPUT_PULLUP);
  }

  // Initialize random number generator with noise from analog pin
  randomSeed(analogRead(A3)); // Use noise from A3 to seed random numbers
}

/*
 * Function: loop
 * -------------
 * This is the main program that runs over and over again
 * Think of it like a never-ending cycle
 *
 * What This Function Does:
 * 1. Checks if the mode button is pressed
 *    - If pressed, switches to the next mode
 *
 * 2. Runs the current mode's function:
 *    - Mode 0 (Manual): Convert binary to decimal/hex
 *    - Mode 1 (Quiz): Practice binary numbers
 *    - Mode 2 (Logic): Try different logic gates
 *    - Mode 3 (ADC): See analog to digital conversion
 *
 * 3. Adds a small delay (100ms) to make everything stable
 */
void loop()
{
  handleModeSwitch(); // Check for mode changes

  // Execute the appropriate mode function
  if (currentMode == 0)
    manualMode(); // Binary conversion mode
  else if (currentMode == 1)
    quizMode(); // Practice quiz mode
  else if (currentMode == 2)
    logicGateMode(); // Logic operations mode
  else if (currentMode == 3)
    adcMode(); // Analog to digital conversion
               // The switch statement can also be used instead of if-else if-else if-else

  delay(100); // Short delay for stability
}

// ——— Mode Switching ———
/*
 * Function: handleModeSwitch
 * -------------------------
 * Manages the mode switching functionality of the system with debounce protection.
 *
 * Technical Details:
 * - Uses hardware debouncing with a delay of debounceDelay milliseconds
 * - Implements state tracking to prevent multiple triggers on a single press
 * - Uses modulo arithmetic for circular mode switching (0 -> 3 -> 0)
 *
 * Modes Available:
 * 0: Manual Mode - Direct control of system functions
 * 1: Quiz Mode   - Interactive questions and answers
 * 2: Logic Mode  - Logic gate demonstration
 * 3: ADC Mode    - Analog-to-Digital conversion display
 *
 * Hardware used:
 * - modeButtonPin: Digital input pin for mode button
 * - LCD Display: 16x2 character LCD for mode indication
 *
 * Global Variables Modified:
 * - currentMode: Current operating mode (0-3)
 * - questionDisplayed: Quiz state tracker
 * - lastButtonState: Previous button state
 * - lastDebounce: Timestamp of last debounce check
 *
 * Note: Function automatically handles LCD updates and quiz state reset
 */
void handleModeSwitch()
{
  bool reading = digitalRead(modeButtonPin); // Sample current button state (LOW when pressed, HIGH when released)

  if (reading != lastButtonState) // Detect button state transition for debounce timing
    lastDebounce = millis();      // Start debounce timer on state change

  if ((millis() - lastDebounce) > debounceDelay) // Process button press only after stable state
  {
    static bool buttonPressed = false; // Static variable to implement press-and-release detection

    if (reading == LOW && !buttonPressed) // Button newly pressed (LOW) and previous press was handled
    {
      buttonPressed = true;                         // Lock out further processing until button release
      currentMode = (currentMode + 1) % totalModes; // Increment mode with wraparound (0->1->2->3->0)
      questionDisplayed = false;                    // Force refresh of quiz display in Quiz mode

      lcd.clear();         // Prepare LCD for new mode display
      lcd.setCursor(0, 0); // Position cursor at start of first LCD line
      switch (currentMode) // Update LCD with new mode name
      {
      case 0:
        lcd.print("Mode: Manual"); // Direct control mode
        break;
      case 1:
        lcd.print("Mode: Quiz"); // Interactive quiz mode
        break;
      case 2:
        lcd.print("Mode: Logic"); // Logic gates demonstration mode
        break;
      case 3:
        lcd.print("Mode: ADC"); // Analog-Digital conversion mode
        break;
      }
    }

    if (reading == HIGH)     // When button is released
      buttonPressed = false; // Reset the button state for next press
  }

  lastButtonState = reading; // Store current state for next iteration's edge detection
}

// ——— Mode 0: Manual Mode ———
/*
 * Function: manualMode
 * -------------------
 * Interactive mode for learning binary number systems and conversions.
 *
 * Technical Implementation:
 * - Uses bitwise operations for efficient binary number construction
 * - Implements LSB-first shift register output
 * - Supports 8-bit binary numbers (0-255 range)
 *
 * Hardware used:
 * - Input: 8 digital switches (switchPins[2-9])
 * - Output:
 *   - 8-bit shift register (74HC595)
 *   - 8 LEDs connected to shift register
 *   - 16x2 LCD display
 * - Control Pins:
 *   - dataPin: Serial data input to shift register
 *   - clockPin: Clock signal for shift register
 *   - latchPin: Storage register clock
 *
 * Display Format:
 * LCD Line 1: "DEC:xxx" (xxx = decimal value 0-255)
 * LCD Line 2: "HEX:0xXX" (XX = hexadecimal value 00-FF)
 *
 * Binary Input Logic:
 * - Switch pressed (LOW)  = Binary 1
 * - Switch released (HIGH) = Binary 0
 * - LSB = Switch 0, MSB = Switch 7
 *
 * Examples:
 * 1. Basic Number:
 *    Switches: 00000001 (Switch 0 pressed)
 *    Display:  DEC:1
 *             HEX:0x01
 *
 * 2. Complex Number:
 *    Switches: 10101010 (Switches 1,3,5,7 pressed)
 *    Display:  DEC:170
 *             HEX:0xAA
 *
 * Note: Leading zeros are automatically added for hex values
 *       below 0x10 to maintain consistent display format
 */
void manualMode()
{
  // Initialize binary value accumulator
  byte state = 0;

  // Read all 8 switches and construct binary number
  for (int i = 0; i < 8; i++)
  {
    if (digitalRead(switchPins[i]) == LOW) // Switch pressed = binary 1
    {
      state |= (1 << i); // Set bit i using bitwise OR and left shift
    }
  }

  // Transfer binary pattern to shift register
  digitalWrite(latchPin, LOW);                  // Prepare for data transmission
  shiftOut(dataPin, clockPin, LSBFIRST, state); // Send bits LSB first
  digitalWrite(latchPin, HIGH);                 // Latch data to display on LEDs

  // Update LCD line 1 with decimal representation
  lcd.setCursor(0, 0);   // Position at start of first line
  lcd.print("DEC:");     // Label for decimal value
  lcd.print((int)state); // Convert byte to decimal
  lcd.print("      ");   // Clear any previous longer numbers

  // Update LCD line 2 with hexadecimal representation
  lcd.setCursor(0, 1); // Position at start of second line
  lcd.print("HEX:0x"); // Standard hex prefix
  if (state < 16)      // Add leading zero for values < 0x10
    lcd.print('0');
  lcd.print(state, HEX); // Convert to uppercase hex
  lcd.print("   ");      // Clear any previous characters
}

// ——— Mode 1: Quiz ———
/*
 * Function: quizMode
 * -----------------
 * Interactive quiz mode for practicing number system conversions between
 * decimal, hexadecimal, and binary.
 *
 * Technical Implementation:
 * - Uses Arduino's random() for target generation (0-255)
 * - Implements bitwise operations for binary input processing
 * - Uses state tracking to manage question lifecycle
 *
 * Quiz Flow:
 * 1. Question Generation:
 *    - Randomly selects target number (0-255)
 *    - Displays target in both decimal and hexadecimal
 *    - Format: "DEC:xxx" and "HEX:0xXX"
 *
 * 2. User Input:
 *    - Student sets switches to represent binary equivalent
 *    - LSB = Switch 0, MSB = Switch 7
 *    - Switch pressed (LOW) = Binary 1
 *    - Switch released (HIGH) = Binary 0
 *
 * 3. Feedback:
 *    - Immediate response on every input change
 *    - "Correct!" displayed for matching input
 *    - "Incorrect" shown for non-matching input
 *    - Auto-generates new question after correct answer
 *
 * Educational Value:
 * - Reinforces understanding of number base conversions
 * - Provides hands-on practice with binary representation
 * - Teaches bitwise operations and digital logic
 *
 * Dependencies:
 * - Global Variables:
 *   - quizTarget: stores current target number
 *   - questionDisplayed: manages question state
 * - Hardware:
 *   - 8 input switches (switchPins[])
 *   - 16x2 LCD display
 */
void quizMode()
{
  // Check if we need to generate and display a new question
  if (!questionDisplayed)
  {
    // Generate new random target (0-255, inclusive)
    quizTarget = random(0, 256);

    // Clear display and show decimal representation
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DEC:");
    lcd.print((int)quizTarget);

    // Show hexadecimal representation with proper formatting
    lcd.setCursor(0, 1);
    lcd.print("HEX:0x");
    if (quizTarget < 16)
      lcd.print('0'); // Add leading zero for values < 0x10
    lcd.print(quizTarget, HEX);

    // Mark question as displayed to prevent regeneration
    questionDisplayed = true;
    return; // Exit to allow user to start inputting answer
  }

  // Read and construct binary number from switch inputs
  byte userInput = 0;
  for (int i = 0; i < 8; i++)
  {
    if (digitalRead(switchPins[i]) == LOW) // Switch pressed = bit set
    {
      userInput |= (1 << i); // Set corresponding bit using bitwise OR
    }
  } 
  
  // Update LED display through 74HC595 shift register
  digitalWrite(latchPin, LOW);    // Pull RCLK low to prevent register updates while shifting
  shiftOut(dataPin, clockPin, LSBFIRST, userInput);  // Send 8 bits serially, LSB first (pin QA = least significant bit)
  digitalWrite(latchPin, HIGH);   // Pull RCLK high to latch data to output pins (QA-QH)

  // Check if user's binary input matches the target
  if (userInput == quizTarget)
  {
    // Display success message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Correct!");
    delay(1000);               // Show success message for 1 second
    questionDisplayed = false; // Trigger new question generation
  }
  else
  {
    // Show incorrect feedback while maintaining question display
    lcd.setCursor(0, 1);
    lcd.print("Incorrect      "); // Extra spaces clear previous text
  }
}

// ——— Mode 2: Logic Gate Mode ———
/*
 * Function: logicGateMode
 * ----------------------
 * An interactive digital logic simulator for learning and experimenting with
 * basic logic gates. Supports 4-bit operations with real-time visual feedback.
 *
 * Technical Implementation:
 * - Uses bitwise operations for logic gate simulation
 * - 4-bit operation limit (values 0-15)
 * - Analog input mapping for gate selection
 * - Real-time binary display via shift register
 *
 * Input Interface:
 * 1. Binary Inputs (4-bit each):
 *    - Input A: Switches 0-3 (LSB to MSB)
 *    - Input B: Switches 4-7 (LSB to MSB)
 *    - Switch pressed (LOW)  = Binary 1
 *    - Switch released (HIGH) = Binary 0
 *
 * 2. Gate Selection:
 *    - Via potentiometer (mapped 0-1023 to 0-3)
 *    - Available Gates:
 *      * AND  (0): A·B      (Set intersection)
 *      * OR   (1): A+B      (Set union)
 *      * XOR  (2): A⊕B      (Exclusive OR)
 *      * XNOR (3): ¬(A⊕B)   (Equivalence)
 *
 * Output Display:
 * 1. LCD Screen:
 *    Line 1: "AAAA OP BBBB"  (Binary inputs and operation)
 *    Line 2: "DEC:X HEX:0xY" (Result in decimal and hex)
 *
 * 2. LED Array:
 *    - Shows operation result in binary
 *    - LSB to MSB arrangement
 *
 * Educational Value:
 * - Demonstrates basic Boolean algebra
 * - Visualizes truth tables in real-time
 * - Teaches binary arithmetic
 * - Shows multiple number representations
 *
 * Example Operations:
 * 1. AND: 1010 AND 1100 = 1000
 *    (Bits set where both inputs are 1)
 * 2. OR:  1010 OR  1100 = 1110
 *    (Bits set where either input is 1)
 * 3. XOR: 1010 XOR 1100 = 0110
 *    (Bits set where inputs differ)
 *
 * Dependencies:
 * - Hardware:
 *   - 8 digital input switches
 *   - Potentiometer on analog input
 *   - 8-bit shift register
 *   - 16x2 LCD display
 * - Constants:
 *   - logicNames[]: Gate operation names
 *   - LogicType enum: Gate type identifiers
 */
void logicGateMode()
{
  // Initialize 4-bit input values from switch states
  byte inputA = 0, inputB = 0;
  for (int i = 0; i < 4; i++)
  {
    // Build first 4-bit number from switches 0-3
    if (digitalRead(switchPins[i]) == LOW) // Switch pressed = bit set
    {
      inputA |= (1 << i); // Set bit using left shift and OR
    }
    // Build second 4-bit number from switches 4-7
    if (digitalRead(switchPins[i + 4]) == LOW)
    {
      inputB |= (1 << i); // Same bit position as A, but from second set of switches
    }
  }

  // Read potentiometer and map its range (0-1023) to gate selection (0-3)
  int potValue = analogRead(potPin);
  byte logicType = map(potValue, 0, 1023, 0, 4); // Maps to GATE_AND through GATE_XNOR

  // Apply selected logic operation to input values
  byte result = 0;
  switch (logicType)
  {
  case GATE_AND:
    result = inputA & inputB; // Bitwise AND: 1 only where both inputs are 1
    break;
  case GATE_OR:
    result = inputA | inputB; // Bitwise OR: 1 where either input is 1
    break;
  case GATE_XOR:
    result = inputA ^ inputB; // Bitwise XOR: 1 where inputs differ
    break;
  case GATE_XNOR:
    result = ~(inputA ^ inputB) & 0x0F; // NOT XOR, masked to 4 bits
    break;
  }

  // Update LED display through shift register
  digitalWrite(latchPin, LOW);                   // Prepare for data transmission
  shiftOut(dataPin, clockPin, LSBFIRST, result); // Send result bits
  digitalWrite(latchPin, HIGH);                  // Latch new data to outputs

  // Format and display inputs and operation on LCD line 1
  lcd.setCursor(0, 0);
  // Display input B in binary (MSB to LSB)
  for (int i = 3; i >= 0; i--)
    lcd.print((inputB >> i) & 1); // Extract each bit
  lcd.print(' ');
  lcd.print(logicNames[logicType]); // Show operation name
  lcd.print(' ');
  // Display input A in binary (MSB to LSB)
  for (int i = 3; i >= 0; i--)
    lcd.print((inputA >> i) & 1);
  lcd.print("  "); // Clear any remnants

  // Show result in decimal and hexadecimal on LCD line 2
  lcd.setCursor(0, 1);
  lcd.print("DEC:");
  lcd.print(result); // Decimal result
  lcd.print(" HEX:0x");
  lcd.print(result, HEX); // Hexadecimal result
  lcd.print("  ");        // Clear any remnants
}

// ——— Mode 3: ADC Mode ———
/*
 * Function: adcMode
 * ----------------
 * Demonstrates Analog-to-Digital Conversion (ADC) principles through real-time
 * visualization of analog sensor readings with multiple display formats.
 *
 * Technical Implementation:
 * - Uses Arduino's 10-bit ADC (0-1023 range)
 * - Performs resolution conversion (10-bit to 8-bit)
 * - Updates at maximum ADC sampling rate
 * - Implements real-time LED visualization
 *
 * Signal Processing:
 * 1. Input Stage:
 *    - Analog Voltage Range: 0-5V
 *    - ADC Resolution: 10 bits (0-1023)
 *    - Sampling: Continuous
 *
 * 2. Conversion Process:
 *    - Input:  10-bit (0-1023)
 *    - Output: 8-bit (0-255)
 *    - Conversion Formula:
 *      output = (input * 255) / 1023
 *
 * Display Outputs:
 * 1. LED Array:
 *    - Shows 8-bit value in binary
 *    - LSB to MSB arrangement
 *    - Real-time updates
 *
 * 2. LCD Screen:
 *    Line 1: "DEC:xxx"    (Decimal, 0-255)
 *    Line 2: "HEX:0xXX"   (Hexadecimal, 00-FF)
 *
 * Educational Value:
 * - Demonstrates ADC resolution concepts
 * - Shows binary weight relationships
 * - Illustrates number base conversions
 * - Visualizes quantization effects
 *
 * Dependencies:
 * Hardware:
 * - Analog sensor on analogSensorPin
 * - 8-bit shift register
 * - 8 LEDs
 * - 16x2 LCD display
 *
 * Note: The ADC conversion time is approximately
 * 100 microseconds (10kHz sampling rate)
 */
void adcMode()
{
  // Sample analog input and convert to digital (10-bit resolution)
  int rawValue = analogRead(analogSensorPin); // Returns 0-1023

  // Convert 10-bit ADC value to 8-bit resolution for LED display
  // Using map() maintains proportional relationship between ranges
  byte mappedValue = map(rawValue, 0, 1023, 0, 255);

  // Update LED binary display through shift register
  digitalWrite(latchPin, LOW);                        // Enable data loading
  shiftOut(dataPin, clockPin, LSBFIRST, mappedValue); // Send bits LSB first
  digitalWrite(latchPin, HIGH);                       // Latch data to outputs

  // Display decimal representation on LCD line 1
  lcd.setCursor(0, 0);    // Position cursor at start
  lcd.print("DEC:");      // Label for decimal value
  lcd.print(mappedValue); // Show 8-bit value (0-255)
  lcd.print("     ");     // Clear any previous digits

  // Display hexadecimal representation on LCD line 2
  lcd.setCursor(0, 1);  // Move to second line
  lcd.print("HEX:0x");  // Standard hex prefix
  if (mappedValue < 16) // Add leading zero for values < 0x10
    lcd.print('0');
  lcd.print(mappedValue, HEX); // Show as uppercase hex
  lcd.print("        ");       // Clear any previous characters
}