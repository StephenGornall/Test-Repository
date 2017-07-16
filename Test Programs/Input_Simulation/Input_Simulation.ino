// ****** Serial Communication Define ******
// Define serial ports used
#define SerialPort1 Serial1

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  SerialPort1.begin(9600);  // Teensy serial interface begin, default speed

}

void loop() {
  // Repeatedly sent example serial data
  Serial.println("*T,R,10,5#");
  SerialPort1.write("*T,R,10,5#");
  delay(2000);
}
