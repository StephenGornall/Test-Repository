// ****** Serial Communication Initialization ******
// Define serial ports used
#define MasterSerialPort Serial1

byte OutgoingMessage;

// ****** Ultrasonic Distance Sensor Varible ******
#define UltrasonicSensorEcho1 A4  // This is the echo pin for the ultrasonic sensor

int UltrasonicSenorAnalogValue1; // This hold the analog read value

float UltrasonicSensorVoltage1; // This holds the calcuated

float UltrasonicSensorDistance1;  // This holds the calcuated distance the nearest object is away.

// ****** Collision Avoidance ******
// Value constraints
uint16_t min_pitch_val = -500;
uint16_t max_pitch_val = 0;

uint16_t PitchValue = 1500; // Stores the pitch value to been sent to the Master Teensy 

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  MasterSerialPort.begin(9600);  // Teensy serial interface begin, default speed

  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON
}

void loop() {
  // Read input from ultrasonic sensor
  UltrasonicSenorAnalogValue1 = analogRead(UltrasonicSensorEcho1);
  Serial.print("Ultrasonic Reading: ");
  Serial.println(UltrasonicSenorAnalogValue1);

  /* 
   *  *** Note *** 
   * This will probably need to be scaled depending on the resulting movement of the vehicle
   * Probably needs to be moved to function for easy editing, with all sensors using the function
   * May need to including the vehicle movement speed in the calculation.
   * 
   * *** Need to test the output ranges and error values ***
   */
  PitchValue = map(UltrasonicSenorAnalogValue1, 0, 1023, min_pitch_val, max_pitch_val);
  
  PitchValue - max_pitch_val;
  Serial.print("Pitch Value: ");
  Serial.println(PitchValue);


  //OutgoingMessage = ("*");
  //OutgoingMessage =+ (PitchValue);
  //OutgoingMessage =+ ("#");
  //Serial.println(OutgoingMessage);
  //MasterSerialPort.write(OutgoingMessage);

  
  Serial.println("*" && PitchValue && "#");
  MasterSerialPort.write("*" && PitchValue && "#");

  /*
   *  
   *  Analogue Minimum: 0
   *  Analogue Maximum: 1023
   *  Distance Minimum: 0cm
   *  Distance Maximum: 600cm
   *  Min pitch: 1000   Moves max backwards
   *  Max Pitch: 2000   Moves max forwards
   *  Mid point: 1500   No input on pitch axis
   *  Min roll: 1000    Moves max left
   *  Max roll: 2000    Moves max right
   *  Mid pointL: 1500  No input on roll axis
   *  
   *  Output for pitch and roll needs to be -500 to 500
   *  So it can be added the the pitch and roll values 
   * 
   *  Range -500 to 500
   *  
   *  Pitch:
   *  0:    No object
   *  -500: Object detected in front, move backwards
   *  500:  Object detected at the rear, move forwards
   *
   *  Roll:
   *  0:    No object
   *  -500: Object detected on the right side, move left
   *  2000  Object detected on the left side, move right
   *  
   *  If all the measurement are greater than 6 meters then the outputs will be 0
   *  
   */

}
