// ****** Serial Communication Initialization ******
// Define serial ports used
#define LinkPROSerialPort Serial4       // LinkPRO is linked to serial port 4

// Serial communication varibles
int IncomingSerialData;     // Stores the current HEX value from the serial
int LinkProBuffer[7];       // Store the current message
int LinkPRO_Message[7][7];  // 2D Array to store in incoming data, 8 bytes, 8 messages
int ByteCounter = 0;        // Counts the number of bytes in the message
boolean LinkPRO_Complete = false; // Indicates when a complete set of data has been recieved

// ****** Function Dump Varibles ******
boolean Handshake_Complete = false;
byte ACK_Message[] = {0x80, 0x00, 0x20, 0x00, 0xFF};

unsigned long Message_Repeat_Counter = 0;

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  LinkPROSerialPort.begin(2400);        // LinkPRO serial interface begin, 2400 speed

  Serial.println("*** Serial Comms Start ***");
  
  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON
}

/*
// Write ACK, 0x00 to LinkPRO
LinkPROSerialPort.write(0x00);
// Write
0x80 0x00 0x20 0x00 0xFF
// Receive
0x80 0x00 0x20 0x00 0xFF
// Write
0x80 0x00 0x20 0x71 0xFF
// Receive
0x80 0x00 0x20 0x71 0xu1 0xu2 0xu3 0xFF
*/
      
void loop() {
  if(Handshake_Complete == false){
    //LinkPROSerialPort.write(ACK_Message, sizeof(ACK_Message));

    /*
    LinkPROSerialPort.write(0x80);
    LinkPROSerialPort.write(0x00);
    LinkPROSerialPort.write(0x20);
    LinkPROSerialPort.write(0x00);
    LinkPROSerialPort.write(0xFF);
    */
    /*
    LinkPROSerialPort.write(B10000000);
    LinkPROSerialPort.write(B00000000);
    LinkPROSerialPort.write(B00100000);
    LinkPROSerialPort.write(B00000000);
    LinkPROSerialPort.write(B11111111);
    */
    
    LinkPROSerialPort.print(byte(0x80));
    LinkPROSerialPort.print(byte(0x00));
    LinkPROSerialPort.print(byte(0x20));
    LinkPROSerialPort.print(byte(0x00));
    LinkPROSerialPort.print(byte(0xFF));

    
    Serial.print(byte(0x80));
    Serial.print(byte(0x00));
    Serial.print(byte(0x20));
    Serial.print(byte(0x00));
    Serial.print(byte(0xFF)); 
    Serial.println();
    
    
    Handshake_Complete = true;
    //Serial.println();
    //Serial.print("Sending: ");
    //Serial.write(ACK_Message, sizeof(ACK_Message));

    /*
    Serial.print(0x80, HEX);
    Serial.print(0x00, HEX);
    Serial.print(0x20, HEX);
    Serial.print(0x00, HEX);
    Serial.print(0xFF, HEX);
    */
    /*
    Serial.write(0x80);
    Serial.write(0x00);
    Serial.write(0x20);
    Serial.write(0x00);
    Serial.write(0xFF);
    */
    /*
    Serial.write(B10000000);
    Serial.write(B00000000);
    Serial.write(B00100000);
    Serial.write(B00000000);
    Serial.write(B11111111);
    */
    
  }
  
  if(LinkPROSerialPort.available() > 0){
    while(LinkPROSerialPort.available()){
      IncomingSerialData = Serial4.read(); // Reads the current HEX value in the data record
      //Serial.print(IncomingSerialData, HEX); // Display to user for debug

      // Look for end of message
      if(IncomingSerialData == 0xFF){
        //Serial.println();
      }
    }
  }

  Message_Repeat_Counter++;
  if(Message_Repeat_Counter >= 200000){
    Handshake_Complete = false;
  }
}





