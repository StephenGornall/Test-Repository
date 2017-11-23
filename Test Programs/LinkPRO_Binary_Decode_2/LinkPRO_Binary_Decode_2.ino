// ****** Serial Communication Initialization ******
// Define serial ports used
#define SerialPort4 Serial4

// Serial communication varibles
int IncomingSerialData;     // Stores the current HEX value from the serial
int LinkPRO_Message[7][7];   // 2D Array to store in incoming data, 8 messages, 8 bytes
bool MessageRecieved = false;  // Used to indicates when there is a message
int ByteCounter = 0;        // Counts the number of bytes in the message
int MessageCounter = 0;     // Counts the number of messages
int MessageType = 0;        // Indicates what sort of message has been recieved

// ****** LinkPRO Varibles ******
int LinkPRO_Voltage;
int LinkPRO_Current;
int LinkPRO_Amphours;
int LinkPRO_State_Of_Charge;
int LinkPRO_Time_To_Charge;
int LinkPRO_Temperature;
int LinkPRO_Monitor_Status;

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  SerialPort4.begin(2400);  // LinkPRO serial interface begin, 2400 speed

  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON

  Serial.println("Startup");
  
}

/*
// ****** LinkPRO Commuincation Information ******
// power up
// few hundred milliseconds delay
0x80 0x00 0x20 0x7F 0xf1 0xf2 0xFF (fx = firmware version)
// one second delay
0x80 0x00 0x20 0x60 0xu1 0xu2 0xu3 0xFF (ux = voltage)
0x80 0x00 0x20 0x61 0xi1 0xi2 0xi3 0xFF (ix = current)
0x80 0x00 0x20 0x62 0xa1 0xa2 0xa3 0xFF (ax = amphours)
0x80 0x00 0x20 0x64 0x%1 0x%2 0x%3 0xFF (%x = state-of-charge)
0x80 0x00 0x20 0x65 0xh1 0xh2 0xh3 0xFF (hx = time-to-go)
0x80 0x00 0x20 0x66 0xt1 0xt2 0xt3 0xFF (tx = temperature)
0x80 0x00 0x20 0x67 0xm1 0xm2 0xm3 0xFF (mx = monitor status)
// Repeats every one second
*/

void loop() {
  // If there is data available on the serial port the program reads it in
  if(SerialPort4.available() > 0){
    while(SerialPort4.available()){
      // Read data from serial port
      IncomingSerialData = Serial4.read(); // Reads the current HEX value in the data record

      // If there is not currently a message, the system looks for the start of the first message
      if(MessageRecieved == false){
        // Look for start of message
        if(IncomingSerialData == 0x80){
          MessageCounter = 0;  // Reset message counter, first message
          ByteCounter = 0;    // Reset byte counter
          
          LinkPRO_Message[MessageCounter][ByteCounter] = IncomingSerialData;
        }
        //else if(IncomingSerialData == 0x00 || IncomingSerialData == 0x20){
          //ByteCounter++;
          //LinkPRO_Message[MessageCounter][ByteCounter] = IncomingSerialData;
        //}
        else if(IncomingSerialData == 0x00){
          ByteCounter = 1;
          LinkPRO_Message[MessageCounter][ByteCounter] = IncomingSerialData;
        }
        else if(IncomingSerialData == 0x20){
          ByteCounter = 2;
          LinkPRO_Message[MessageCounter][ByteCounter] = IncomingSerialData;
        }
        else if(IncomingSerialData == 0x60){
          ByteCounter = 3;
          MessageRecieved = true;
        }
      }

      // If there is a message, then -->
      if(MessageRecieved == true){
        Serial.print(IncomingSerialData, HEX);
        Serial.print(" ");

        LinkPRO_Message[MessageCounter][ByteCounter] = IncomingSerialData;
        
        ByteCounter++;
        
        // Detect end of message
        if(IncomingSerialData == 0xFF){
          ByteCounter = 0;
          Serial.print(MessageCounter);
          Serial.println();
          if(MessageCounter <= 7){
            MessageCounter++;
          }
          else{
              MessageRecieved = false;
              // *** Print to user for debug ***
              Serial.println();
              for(int y = 0; y <= 7; y++){
                for(int x = 0; x <= 7; x++){
                  Serial.print("[");
                  Serial.print(y);
                  Serial.print(",");
                  Serial.print(x);
                  Serial.print("] ");
                  Serial.print(LinkPRO_Message[y][x], HEX);
                  Serial.print(" ");
                }
                Serial.println();
              }
          }
        }
        
      }
    }
  }
}
