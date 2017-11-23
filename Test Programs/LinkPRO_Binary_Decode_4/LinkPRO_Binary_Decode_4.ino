#pragma pack(1)

// ****** Serial Communication Initialization ******
// Define serial ports used
#define MasterTeensySerialPort Serial1  // Master Teensy is linked to serial port 1 
#define LinkPROSerialPort Serial4       // LinkPRO is linked to serial port 4

// Serial communication varibles
int IncomingSerialData;     // Stores the current HEX value from the serial
int LinkProBuffer[7];       // Store the current message
int LinkPRO_Message[7][7];  // 2D Array to store in incoming data, 8 bytes, 8 messages
int ByteCounter = 0;        // Counts the number of bytes in the message
int MessageCounter = 0;     // Counts the number of messages
int MessageType = 0;        // Indicates what sort of message has been recieved
boolean LinkPRO_Complete = false; // Indicates when a complete set of data has been recieved


// ****** LinkPRO Varibles ******
uint16_t LinkPRO_Voltage;
uint16_t LinkPRO_Current;
uint16_t LinkPRO_Amphours;
uint16_t LinkPRO_State_Of_Charge;
uint16_t LinkPRO_Time_To_Charge;
uint16_t LinkPRO_Temperature;
uint16_t LinkPRO_Monitor_Status;


// ****** LinkPRO Struct ******
struct{
  uint16_t Voltage;
  uint16_t Current;
  uint16_t Amphours;
  uint16_t State_Of_Charge;
  uint16_t Time_To_Charge;
  uint16_t Temperature;
  uint16_t Monitor_Status;
} LinkPR0_Data;

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  MasterTeensySerialPort.begin(57600);  // Master serial interface begin, 57600 speed
  LinkPROSerialPort.begin(2400);        // LinkPRO serial interface begin, 2400 speed

  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON
}


/*
// ****** LinkPRO Commuincation Information ******
// Power up
// few hundred milliseconds delay
0x80 0x00 0x20 0x7F 0xf1 0xf2 0xFF (fx = firmware version)
// One second delay
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
  // If there is data available on the LinkPRO serial port the program reads it in
  if(LinkPROSerialPort.available() > 0){
    //Serial.print("Incoming "); // Display to user for debug ***
    while(LinkPROSerialPort.available()){
      // Read data from serial port
      IncomingSerialData = Serial4.read(); // Reads the current HEX value in the data record
      Serial.print(IncomingSerialData, HEX); // Display to user for debug
      ByteCounter++;  // Increment byte counter, new byte recieved
      
      // Error check, incoming bytes, messages are 8 bytes long
      if(ByteCounter > 8){
        //Serial.println();
        //Serial.println("Error: Too many bytes in LinkPRO message"); ****** Error Minor ******
      }
      
      // Look for start of message
      if(IncomingSerialData == 0x80){
        ByteCounter = 0;   // Reset byte counter
      }

      Serial.print(" :");
      Serial.print(MessageCounter);
      Serial.print(": ");
      
      LinkProBuffer[ByteCounter] = IncomingSerialData;

      
      // Check the message type
      if(ByteCounter == 3){
        Serial.print(" : Triggered = ");
        if(IncomingSerialData == 0x60){
          MessageCounter = 0;
         }
        else if(IncomingSerialData == 0x61){
          MessageCounter = 1;
        }
        else if(IncomingSerialData == 0x62){
          MessageCounter = 2;
        }
        else if(IncomingSerialData == 0x64){
          MessageCounter = 3;
        }
        else if(IncomingSerialData == 0x65){
          MessageCounter = 4;
        }
        else if(IncomingSerialData == 0x66){
          MessageCounter = 5;
        }
        else if(IncomingSerialData == 0x67){
          MessageCounter = 6;
        }
        else if(IncomingSerialData == 0x68){
          MessageCounter = 7;
          LinkPRO_Complete = true;
        }
        else{
          Serial.println(" <-- Error: Wrong message ID");
        }
        Serial.print(MessageCounter);
        Serial.print(" :");
      }
      
      
      
      // Look for end of message
      if(IncomingSerialData == 0xFF){
        //Serial.println();  // Print new line
        //Serial.print("Message Counter: ");
        //Serial.println(MessageCounter);
        //Serial.println("Buffer pre-2D");
        for(int i = 0; i <= 7; i++){
          //Serial.print(i);
          //Serial.print(": ");
          //Serial.println(LinkProBuffer[i], HEX);
        }

        /*
        // Check the message type
        if(LinkProBuffer[3] == 0x60){
          MessageCounter = 0;
         }
        else if(LinkProBuffer[3] == 0x61){
          MessageCounter = 1;
        }
        else if(LinkProBuffer[3] == 0x62){
          MessageCounter = 2;
        }
        else if(LinkProBuffer[3] == 0x64){
          MessageCounter = 3;
        }
        else if(LinkProBuffer[3] == 0x65){
          MessageCounter = 4;
        }
        else if(LinkProBuffer[3] == 0x66){
          MessageCounter = 5;
        }
        else if(LinkProBuffer[3] == 0x67){
          MessageCounter = 6;
        }
        else if(LinkProBuffer[3] == 0x68){
          MessageCounter = 7;
          LinkPRO_Complete = true;
        }
        else{
          // Serial.println(" <-- Error: Wrong message ID");  // ****** Error Minor ******
        }
        */        
       
        // Store current message in the buffer to the data array
        for(int i = 0; i <= 7; i++){
          LinkPRO_Message[i][MessageCounter] = LinkProBuffer[i];
          //Serial.print(i);
          //Serial.print(": ");
          //Serial.println(LinkProBuffer[i], HEX);
        }
              
        if(LinkPRO_Complete == true){
          Print_LinkPRO_Data();
          Decode_LinkPRO_Data();
          Send_LinkPRO_Data();
          LinkPRO_Complete = false;
          Serial.println();  // Print new line
          //Serial.println("Raw message from LinkPRO:");
        }
        Serial.println();
      }
    }
  }
}

/*
uint16_t LinkPRO_Voltage;
uint16_t LinkPRO_Current;
uint16_t LinkPRO_Amphours;
uint16_t LinkPRO_State_Of_Charge;
uint16_t LinkPRO_Time_To_Charge;
uint16_t LinkPRO_Temperature;
uint16_t LinkPRO_Monitor_Status;
*/

// Decode LinkPRO Data
void Decode_LinkPRO_Data(){
  Serial.println();
  // Decode voltage
  LinkPRO_Voltage = (LinkPRO_Message[4][0] << 14)+(LinkPRO_Message[5][0] << 7) + LinkPRO_Message[6][0];
  LinkPRO_Current = (LinkPRO_Message[4][1] << 14)+(LinkPRO_Message[5][1] << 7) + LinkPRO_Message[6][1];
  LinkPRO_Amphours = (LinkPRO_Message[4][2] << 14)+(LinkPRO_Message[5][2] << 7) + LinkPRO_Message[6][2];
  LinkPRO_State_Of_Charge = (LinkPRO_Message[4][3] << 14)+(LinkPRO_Message[5][3] << 7) + LinkPRO_Message[6][3];
  LinkPRO_Time_To_Charge = (LinkPRO_Message[4][4] << 14)+(LinkPRO_Message[5][4] << 7) + LinkPRO_Message[6][4];
  LinkPRO_Temperature = (LinkPRO_Message[4][5] << 14)+(LinkPRO_Message[5][5] << 7) + LinkPRO_Message[6][5];
  LinkPRO_Monitor_Status = (LinkPRO_Message[4][6] << 14)+(LinkPRO_Message[5][6] << 7) + LinkPRO_Message[6][6];
  Serial.println(LinkPRO_Voltage);
  Serial.println(LinkPRO_Current);
  Serial.println(LinkPRO_Amphours);
  Serial.println(LinkPRO_State_Of_Charge);
  Serial.println(LinkPRO_Time_To_Charge);
  Serial.println(LinkPRO_Monitor_Status);
}

// Send LinkPRO Data to Master Teensy
void Send_LinkPRO_Data() {
  // Master Teensy serial connection **********************
  // Error handling *****************
}

// *** Print to user for debug ***
void Print_LinkPRO_Data() {
  Serial.println();
  Serial.println("Stored LinkPRO data:");
  for(int y = 0; y <= 7; y++){
    for(int x = 0; x <= 7; x++){
      Serial.print("[");
      Serial.print(x);
      Serial.print(",");
      Serial.print(y);
      Serial.print("] ");
      Serial.print(LinkPRO_Message[x][y], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println();
}




