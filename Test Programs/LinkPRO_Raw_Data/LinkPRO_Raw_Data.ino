// ****** Serial Communication Initialization ******
// Define serial ports used
#define LinkPROSerialPort Serial4       // LinkPRO is linked to serial port 4

// Serial communication varibles
int IncomingSerialData;     // Stores the current HEX value from the serial

boolean Setup = false;

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  LinkPROSerialPort.begin(2400);        // LinkPRO serial interface begin, 2400 speed

  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON

  delay(1000);
}

void loop() {
  if(Setup == false){
    //LinkPROSerialPort.println(0x23);  // Backlight on, command 0x23
    //LinkPROSerialPort.write(0x27);  // // Request mode, not everysecond, command 0x27
    Setup = true;
  }
  
  // If there is data available on the LinkPRO serial port the program reads it in
  if(LinkPROSerialPort.available() > 0){
    //Serial.print("Incoming "); // Display to user for debug ***
    while(LinkPROSerialPort.available()){
      // Read data from serial port
      IncomingSerialData = Serial4.read(); // Reads the current HEX value in the data record
      Serial.print(IncomingSerialData, HEX); // Display to user for debug

      // Look for end of message
      if(IncomingSerialData == 0xFF){
        Serial.println();
      }
    }  
  }
}
