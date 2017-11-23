// ****** Serial Communication Initialization ******
// Define serial ports used
#define SerialPort4 Serial4

// Serial communication varibles
int IncomingSerialData;     // Stores the current HEX value from the serial
int LinkProBuffer[7];       // Store the current message
int LinkPRO_Message[7][7];  // 2D Array to store in incoming data, 8 bytes, 8 messages
int ByteCounter = 0;        // Counts the number of bytes in the message
int MessageCounter = 0;     // Counts the number of messages
int MessageType = 0;        // Indicates what sort of message has been recieved
boolean LinkPRO_Complete = false; // Indicates when a complete set of data has been recieved

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
      Serial.print(IncomingSerialData, HEX); // Display to user for debug
      ByteCounter++;  // Increment byte counter, new byte recieved
      
      // Error check, incoming bytes, messages are 8 bytes long
      if(ByteCounter > 8){
        Serial.println();
        Serial.println("Error: Too many bytes in LinkPRO message");
      }
      
      // Look for start of message
      if(IncomingSerialData == 0x80){
        ByteCounter = 0;   // Reset byte counter
      }

      LinkProBuffer[ByteCounter] = IncomingSerialData;
      //Serial.print(LinkProBuffer[ByteCounter], HEX);
      
      // Look for end of message
      if(IncomingSerialData == 0xFF){
        Serial.println();  // Print new line
        // Check the message type
        if(LinkProBuffer[3] == 0x60){
          MessageCounter = 0;
         }
        if(LinkProBuffer[3] == 0x61){
          MessageCounter = 1;
        }
        if(LinkProBuffer[3] == 0x62){
          MessageCounter = 2;
        }
        if(LinkProBuffer[3] == 0x64){
          MessageCounter = 3;
        }
        if(LinkProBuffer[3] == 0x65){
          MessageCounter = 4;
        }
        if(LinkProBuffer[3] == 0x66){
          MessageCounter = 5;
        }
        if(LinkProBuffer[3] == 0x67){
          MessageCounter = 6;
        }
        if(LinkProBuffer[3] == 0x68){
          MessageCounter = 7;
          LinkPRO_Complete = true;
        }
        
        // Error check, is the default message? Or response? **********************
        
        
        // Store current message in the buffer to the data array
        for(int i = 0; i <= 7; i++){
          LinkPRO_Message[i][MessageCounter] = LinkProBuffer[i];
          Serial.println(LinkProBuffer[i], HEX);
        }
              
        if(LinkPRO_Complete == true){
          Print_LinkPRO_Data();
          Send_LinkPRO_Data();
          LinkPRO_Complete = false;
          Serial.println();  // Print new line
          Serial.println("Raw message from LinkPRO:");
        }
      }
    }
  }
}

// *** Print to user for debug ***
void Print_LinkPRO_Data() {
  Serial.println();
  Serial.println("Stored LinkPRO data:");
  for(int x = 0; x < 8; x++){
          Serial.println(LinkPRO_Message[x][0], HEX);
  }
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

// Send LinkPRO Data to Master Teensy
void Send_LinkPRO_Data() {
  // Master Teensy serial connection **********************
  // Error handling *****************
}

