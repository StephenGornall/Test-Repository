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
boolean LinkPRO_Complete = false; // Indicates when a complete set of data has been recieved

// ****** LinkPRO Varibles ******
int LinkPRO_Voltage[7];
int LinkPRO_Current[7];
int LinkPRO_Amphours[7];
int LinkPRO_State_Of_Charge[7];
int LinkPRO_Time_To_Charge[7];
int LinkPRO_Temperature[7];
int LinkPRO_Monitor_Status[7];
int LinkPRO_Other[7];

// ****** LinkPRO Struct ******
struct{
  String Identifier;  // Change to string
  uint16_t Voltage;
  uint16_t Current;
  uint16_t Amphours;
  uint16_t State_Of_Charge;
  uint16_t Time_To_Charge;
  uint16_t Temperature;
  uint16_t Monitor_Status;
  uint16_t CheckSum;
} LinkPR0_Data;

// ****** Thermistor Varibles ******
const int ThermistorSeriesResistorValue = 10000; // Define value of thermistor series resistor

// Define thermistor pins
const int Thermistor1 = A0;
const int Thermistor2 = A1;
const int Thermistor3 = A2;
const int Thermistor4 = A3;

float ThermistorReading[4]; // Array of thermistor readings

// Define sample varibles
const int NumberofThermistorSamples = 5;
// Sample array for each thermistor
int Thermistor_Samples_1[NumberofThermistorSamples];
int Thermistor_Samples_2[NumberofThermistorSamples];
int Thermistor_Samples_3[NumberofThermistorSamples];
int Thermistor_Samples_4[NumberofThermistorSamples];
int SampleCounter;  // Counts number of samples
// Varibles used to calculate and store average thermistor values
float Average_Thermistor_Reading_1;
float Average_Thermistor_Reading_2;
float Average_Thermistor_Reading_3;
float Average_Thermistor_Reading_4;

// ****** Thermistor Struct ******
struct{
  String Identifier;
  uint16_t Thermistor1;
  uint16_t Thermistor2;
  uint16_t Thermistor3;
  uint16_t Thermistor4;
  uint16_t CheckSum;
} Thermistor_Data;

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

  LinkPR0_Data.Identifier = "LinkPRO";
  LinkPR0_Data.CheckSum = 10;
  Thermistor_Data.Identifier = "Thermistor";
  Thermistor_Data.CheckSum = 10;
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

// Decode LinkPRO Data
void Decode_LinkPRO_Data(){
  Serial.println();
  // Voltage Decode
  LinkPR0_Data.Voltage = (LinkPRO_Voltage[4] << 14) + (LinkPRO_Voltage[5] << 7) + LinkPRO_Voltage[6];
  // Current Decode
  LinkPR0_Data.Current = (LinkPRO_Current[4] << 14) + (LinkPRO_Current[5] << 7) + LinkPRO_Current[6];
  // Amphours Decode
  LinkPR0_Data.Amphours = (LinkPRO_Amphours[4] << 14) + (LinkPRO_Amphours[5] << 7) + LinkPRO_Amphours[6];
  // State Of Charge Decode
  LinkPR0_Data.State_Of_Charge = (LinkPRO_State_Of_Charge[4] << 14) + (LinkPRO_State_Of_Charge[5] << 7) + LinkPRO_State_Of_Charge[6];
  // Time To Charge Decode
  LinkPR0_Data.Time_To_Charge = (LinkPRO_Time_To_Charge[4] << 14) + (LinkPRO_Time_To_Charge[5] << 7) + LinkPRO_Time_To_Charge[6];
  // Temperature Decode
  LinkPR0_Data.Temperature = (LinkPRO_Temperature[4] << 14) + (LinkPRO_Temperature[5] << 7) + LinkPRO_Temperature[6];
  // Monitor Status Decode
  LinkPR0_Data.Monitor_Status = (LinkPRO_Monitor_Status[4] << 14) + (LinkPRO_Monitor_Status[5] << 7) + LinkPRO_Monitor_Status[6];
  // 'Other' missing, not needed. Message 0x68 does not appear in data sheet
  Serial.print("Voltage Decoded: ");
  Serial.println(LinkPR0_Data.Voltage);
  Serial.print("Current Decoded: ");
  Serial.println(LinkPR0_Data.Current);
  Serial.print("Amphours Decoded: ");
  Serial.println(LinkPR0_Data.Amphours);
  Serial.print("State Of Charge Decoded: ");
  Serial.println(LinkPR0_Data.State_Of_Charge);
  Serial.print("Current Time To Charge: ");
  Serial.println(LinkPR0_Data.Time_To_Charge);
  Serial.print("Monitor Status Decoded: ");
  Serial.println(LinkPR0_Data.Monitor_Status);
}

// Send LinkPRO Data to Master Teensy
void Send_LinkPRO_Data() {
  // Error handling *****************

  char Buffer[sizeof(LinkPR0_Data) + 1];  // Why + 1?? *******

  memcpy(Buffer, &LinkPR0_Data, sizeof(LinkPR0_Data));
  
  MasterTeensySerialPort.write(Buffer, (sizeof(MasterTeensySerialPort)));
  
  /*
  MasterTeensySerialPort.print("Voltage Decoded: ");
  MasterTeensySerialPort.println(LinkPR0_Data.Voltage);
  MasterTeensySerialPort.print("Current Decoded: ");
  MasterTeensySerialPort.println(LinkPR0_Data.Current);
  MasterTeensySerialPort.print("Amphours Decoded: ");
  MasterTeensySerialPort.println(LinkPR0_Data.Amphours);
  MasterTeensySerialPort.print("State Of Charge Decoded: ");
  MasterTeensySerialPort.println(LinkPR0_Data.State_Of_Charge);
  MasterTeensySerialPort.print("Current Time To Charge: ");
  MasterTeensySerialPort.println(LinkPR0_Data.Time_To_Charge);
  MasterTeensySerialPort.print("Monitor Status Decoded: ");
  MasterTeensySerialPort.println(LinkPR0_Data.Monitor_Status);
  */
}

// Calculate Average Thermistor Reading
void Average_Thermistor_Readings(){
  // Calculate Average thermistor reading
  for(SampleCounter = 0; SampleCounter < NumberofThermistorSamples; SampleCounter++){
    Average_Thermistor_Reading_1 = Average_Thermistor_Reading_1 + Thermistor_Samples_1[NumberofThermistorSamples];
    Average_Thermistor_Reading_2 = Average_Thermistor_Reading_2 + Thermistor_Samples_2[NumberofThermistorSamples];
    Average_Thermistor_Reading_3 = Average_Thermistor_Reading_3 + Thermistor_Samples_3[NumberofThermistorSamples];
    Average_Thermistor_Reading_4 = Average_Thermistor_Reading_4 + Thermistor_Samples_4[NumberofThermistorSamples];
  }
  Average_Thermistor_Reading_1 = Average_Thermistor_Reading_1 / NumberofThermistorSamples;
  Average_Thermistor_Reading_2 = Average_Thermistor_Reading_2 / NumberofThermistorSamples;
  Average_Thermistor_Reading_3 = Average_Thermistor_Reading_3 / NumberofThermistorSamples;
  Average_Thermistor_Reading_4 = Average_Thermistor_Reading_4 / NumberofThermistorSamples;
  
  // ****** Debug ******
  Serial.print("Thermistor 1 averaged reading: ");
  Serial.println(Average_Thermistor_Reading_1);
  Serial.print("Thermistor 2 averaged reading: ");
  Serial.println(Average_Thermistor_Reading_2);
  Serial.print("Thermistor 3 averaged reading: ");
  Serial.println(Average_Thermistor_Reading_3);
  Serial.print("Thermistor 4 averaged reading: ");
  Serial.println(Average_Thermistor_Reading_4);
  
  // Convert the analog reading resistance to a resistance
  Average_Thermistor_Reading_1 = (1023/Average_Thermistor_Reading_1) - 1;
  Average_Thermistor_Reading_2 = (1023/Average_Thermistor_Reading_2) - 1;
  Average_Thermistor_Reading_3 = (1023/Average_Thermistor_Reading_3) - 1;
  Average_Thermistor_Reading_4 = (1023/Average_Thermistor_Reading_4) - 1;

  Thermistor_Data.Thermistor1 = Average_Thermistor_Reading_1;
  Thermistor_Data.Thermistor2 = Average_Thermistor_Reading_2;
  Thermistor_Data.Thermistor3 = Average_Thermistor_Reading_3;
  Thermistor_Data.Thermistor4 = Average_Thermistor_Reading_4;

  Serial.print("Thermistor 1 resistance value: ");
  Serial.println(Thermistor_Data.Thermistor1);
  Serial.print("Thermistor 2 resistance value: ");
  Serial.println(Thermistor_Data.Thermistor2);
  Serial.print("Thermistor 3 resistance value: ");
  Serial.println(Thermistor_Data.Thermistor3);
  Serial.print("Thermistor 4 resistance value: ");
  Serial.println(Thermistor_Data.Thermistor4);
}

// Sends thermistor data to master teensy
void Send_Thermistor_Data(){
  // Error handling *****************

  char Buffer[sizeof(Thermistor_Data) + 1];  // Why + 1?? *******

  memcpy(Buffer, &Thermistor_Data, sizeof(Thermistor_Data));
  
  MasterTeensySerialPort.write(Buffer, (sizeof(MasterTeensySerialPort)));
}

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
        Serial.println();
        Serial.println("Error: Too many bytes in LinkPRO message"); // ****** Error Minor ******
      }
      
      // Look for start of message
      if(IncomingSerialData == 0x80){
        ByteCounter = 0;   // Reset byte counter
      }
     
      LinkProBuffer[ByteCounter] = IncomingSerialData;
      //Serial.print(LinkProBuffer[ByteCounter], HEX);
      
      // Look for end of message
      if(IncomingSerialData == 0xFF){
        Serial.println();
        // Check the message type
        //Message_Counter++;
        if(LinkProBuffer[3] == 0x60){
          //Message_Counter = 0;
          Serial.print("Voltage Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Voltage[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Voltage[i]);
          }
        }
        else if(LinkProBuffer[3] == 0x61){
          Serial.print("Current Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Current[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Current[i]);
          }
        }
        else if(LinkProBuffer[3] == 0x62){
          Serial.print("Amphours Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Amphours[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Amphours[i]);
          }
        }
        else if(LinkProBuffer[3] == 0x64){
          Serial.print("State Of Change Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_State_Of_Charge[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_State_Of_Charge[i]);
          }
        }
        else if(LinkProBuffer[3] == 0x65){
          Serial.print("Time To Charge Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Time_To_Charge[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Time_To_Charge[i]);
          }
        }
        else if(LinkProBuffer[3] == 0x66){
          Serial.print("Temperature Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Temperature[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Temperature[i]);
          }
        }
        else if(LinkProBuffer[3] == 0x67){
          Serial.print("Monitor Status Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Monitor_Status[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Monitor_Status[i]);
          }
        }
        else if(LinkProBuffer[3] == 0x68){
          Serial.print("Other Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Other[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Other[i]);
          }
          LinkPRO_Complete = true;
        }
        else{
          Serial.print(" <-- Error: Wrong message ID");  // ****** Error Minor ******
        }
                      
        //if(LinkPRO_Complete == true && Message_Counter == 7){
        if(LinkPRO_Complete == true){
          Serial.println();
          Decode_LinkPRO_Data();
          Send_LinkPRO_Data();
        }
        LinkPRO_Complete = false;
        Serial.println();
      }
    }
  }

  
    
  if(SampleCounter <= NumberofThermistorSamples){
    // Read each thermistor and stores it in the arrays
    Thermistor_Samples_1[SampleCounter] = analogRead(Thermistor1);
    Thermistor_Samples_2[SampleCounter] = analogRead(Thermistor2);
    Thermistor_Samples_3[SampleCounter] = analogRead(Thermistor3);
    Thermistor_Samples_4[SampleCounter] = analogRead(Thermistor4);
    SampleCounter++;  // Increase sample counter
  }
  else{
    Average_Thermistor_Readings();
    Send_Thermistor_Data();
    SampleCounter = 0;  // Reset sample counter
  }
}



