// ****** Ultrasonic Distance Sensor Initialization ******
// Defines all nine Ultrasonic Sensors
#define FrontLeftUltrasonicSensorSignal A0  // This is the echo pin for the ultrasonic sensor
#define FrontRightUltrasonicSensorSignal A1  // This is the echo pin for the ultrasonic sensor
#define LeftFrontUltrasonicSensorSignal A2  // This is the echo pin for the ultrasonic sensor
#define RightFrontUltrasonicSensorSignal A3  // This is the echo pin for the ultrasonic sensor
#define LeftRearUltrasonicSensorSignal A4  // This is the echo pin for the ultrasonic sensor
#define RightRearUltrasonicSensorSignal A5  // This is the echo pin for the ultrasonic sensor
#define UltrasonicSensorSignal A6  // This is the echo pin for the ultrasonic sensor
#define UltrasonicSensorSignal A7  // This is the echo pin for the ultrasonic sensor
#define UltrasonicSensorSignal A0  // This is the echo pin for the ultrasonic sensor

// Need to be changed to include direction the sensor is facing ***************

// Arrays to hold sensor data
int UltrasonicSenorAnalogValue[8]; // This holds the analog value read from each of the sensors
float UltrasonicSensorVoltage[8]; // This hold the voltages calculated from the sensor data
float UltrasonicSensorDistance[8];  // This holds the calculated distance from the sensor data

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

void setup(){
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed

  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON
}

void UltrasonicSensorRead(){
  
  
}

void loop() {
  

}


