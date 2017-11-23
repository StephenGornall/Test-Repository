// ****** Ultrasonic Distance Sensor Varible ******
#define UltrasonicSensorEcho1 A4  // This is the echo pin for the ultrasonic sensor

int UltrasonicSenorAnalogValue1; // This hold the analog read value

float UltrasonicSensorVoltage1; // This holds the calcuated

float UltrasonicSensorDistance1;  // This holds the calcuated distance the nearest object is away.

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed

  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON
}

void loop() {
  // Read input from ultrasonic sensor
  UltrasonicSenorAnalogValue1 = analogRead(UltrasonicSensorEcho1);
  Serial.print("Ultrasonic Reading: ");
  Serial.println(UltrasonicSenorAnalogValue1);  
  
  // Convert reading to voltage
  UltrasonicSensorVoltage1 = (UltrasonicSenorAnalogValue1 * (3.3/1023.0));
  //Serial.print("Ultrasonic Voltage: ");
  //Serial.println(UltrasonicSensorVoltage1);

  // Convert voltage to distance, in cm
  // According to the data sheet using 3.3V: 3.2mV/cm
  UltrasonicSensorDistance1 = (UltrasonicSensorVoltage1/0.0032);
  //Serial.print("Ultrasonic Distance: ");
  //Serial.println(UltrasonicSensorDistance1);

  //delay(200);
}
