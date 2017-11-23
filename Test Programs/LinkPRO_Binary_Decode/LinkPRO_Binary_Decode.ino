// ****** Serial Communication Initialization ******
// Define serial ports used
#define SerialPort4 Serial4

// Serial communication varibles
int IncomingSerialData;  // Stores the current HEX value from the serial 
int MessageCounter = 0;  // This is the counter used to read the HEX characters from the serial port
int MessageType = 0;     // Indicates what sort of message has been recieved

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

So, not interested in

We are interested in the 4th message, the message identifier 0x60 to 0x67.
Then the next 

*/


void loop() {
  // If there is data available on the serial port the program reads it in
  if(SerialPort4.available() > 0){
    //Serial.print("Incoming "); // Display to user for debug ***
    while(SerialPort4.available()){
      // Read data from serial port
      IncomingSerialData = Serial4.read(); // Reads the current HEX value in the data record
      MessageCounter++;
      
      // Look for start of message
      if(IncomingSerialData == 0x80){
        MessageCounter = 1;  // Reset message counter, first message
             
      }
            
      // The are not interested in data chucks 2 and 3
      //if(IncomingSerialData == 0x00 || IncomingSerialData == 0x20){

      //}

      // Byte 4 is the message identifier
      if(MessageCounter == 4){
        // Message type detection
        if(IncomingSerialData == 0x60){
          MessageType = 0; // Voltage
          //Serial.print(" T0 ");
          
        }
    
        else if(IncomingSerialData == 0x61){
          MessageType = 1; // Current
          //Serial.print(" T1 ");
  
        }
  
        else if(IncomingSerialData == 0x62){
          MessageType = 2; // Amphours
          //Serial.print(" T2 ");
  
        }
  
        else if(IncomingSerialData == 0x64){
          MessageType = 4; // State-of-charge
          //Serial.print(" T4 ");
  
        }
  
        else if(IncomingSerialData == 0x65){
          MessageType = 5; // Time-to-go
          //Serial.print(" T5 ");
  
        }
  
        else if(IncomingSerialData == 0x66){
          MessageType = 6; // Temperature
          //Serial.print(" T6 ");
  
        }
  
        else if(IncomingSerialData == 0x67){
          MessageType = 7; // Monitor Status
          //Serial.print(" T7 ");
          
        }
        
        else if(IncomingSerialData == 0x68){
          MessageType = 8; // ??????
          //Serial.print(" T8 ");
          
        }

        else{
          Serial.print(" Error ");
        }
      }

      

      Serial.print(" ");  // Print a space between the two values
      Serial.print(IncomingSerialData, HEX); // Display to user for debug

      // Detect end of message
      if(IncomingSerialData == 0xFF){
        Serial.print(" ");  // Print a space between the two values
        Serial.print(MessageType); // Display to user for debug
        Serial.println(); // Print new line between data chucks
      }
      
      switch(MessageType){
        case 0:
          // Voltage
          if(MessageCounter == 5){
            LinkPRO_Voltage = IncomingSerialData;
          }
          if(MessageCounter == 6){
            LinkPRO_Voltage =+ IncomingSerialData;
          }
          if(MessageCounter == 7){
            LinkPRO_Voltage =+ IncomingSerialData;
          }
          if(MessageCounter == 8){
            LinkPRO_Voltage =+ IncomingSerialData;
          }
          break;
        case 1:
          // Current
          break;
        case 2:
          // Amphours
          break;
        case 4:
          // State-of-charge
          break;
        case 5:
          // Time-to-go
          break;
        case 6:
          // Temperature
          break;
        case 7:
          // Monitor Status
          break;
        case 8:
          // ??????
          if(IncomingSerialData == 0xFF){
            Serial.println();
            Serial.println();
          }
          break;
      }
    }
  }
}
