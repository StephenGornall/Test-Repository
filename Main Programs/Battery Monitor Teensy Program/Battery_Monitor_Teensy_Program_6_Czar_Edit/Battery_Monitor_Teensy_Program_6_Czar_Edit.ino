// ****** Include Libraries ******
// Libararies used for SD card storage
#include <SD.h>
#include <SPI.h> 
#include <Common.h>
#pragma pack(1) // Fixes serial communication error

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

// Serial Message Identifiers
/*
enum{
  MASTER = 0,
  JOYSTICK,
  COL_AVOID,
  BATT_FS_TEMP
} sys_id;
*/

// ****** LinkPRO Varibles ******
int LinkPRO_Voltage[7];
int LinkPRO_Current[7];
int LinkPRO_Amphours[7];
int LinkPRO_State_Of_Charge[7];
int LinkPRO_Time_To_Charge[7];
int LinkPRO_Temperature[7];
int LinkPRO_Monitor_Status[7];
int LinkPRO_Other[7];
int Message_Counter = 0;

// ****** LinkPRO Struct ******
struct{
  uint16_t Identifier; 
  uint16_t Voltage;
  uint16_t Current;
  uint16_t Amphours;
  uint16_t State_Of_Charge;
  uint16_t Time_To_Charge;
  uint16_t Temperature;
  uint8_t Monitor_Status[16];
} LinkPRO_Data;

// ****** Thermistor Varibles ******
const int ThermistorSeriesResistorValue = 10000; // Define value of thermistor series resistor

// Define thermistor pins
const int Thermistor1 = A0;
const int Thermistor2 = A1;
const int Thermistor3 = A2;
const int Thermistor4 = A3;

float ThermistorReading[4]; // Array of thermistor readings
// Stores the cumulative values collected from the thermistors
int Thermistor_Samples_1;
int Thermistor_Samples_2;
int Thermistor_Samples_3;
int Thermistor_Samples_4;
int SampleCounter = 0;  // Counts number of samples
// Varibles used to calculate and store average thermistor values
float Average_Thermistor_Reading_1;
float Average_Thermistor_Reading_2;
float Average_Thermistor_Reading_3;
float Average_Thermistor_Reading_4;

// ****** Thermistor Struct ******
struct{
  uint8_t Identifier;
  uint8_t Thermistor1;
  uint8_t Thermistor2;
  uint8_t Thermistor3;
  uint8_t Thermistor4;
} Thermistor_Data;

// ****** System Control Varibles ******
// Input
const int LiPo_Charger_Input = 5; // Attached to the state indicator LED on the RC charger, HIGH when charging ****** correct?
const int S_DOI_1 = 35;           // Monitors the state of the DC to DC 1, HIGH when working
const int S_DOI_2 = 36;           // Monitors the state of the DC to DC 2, HIGH when working
const int CELL_VOLTAGE_LOW = 37;  // Indicates when battery cell voltage is low, HIGH whens cells are low
// Output
const int DOR_3 = 38;             // Set high when DC to DC 1 fails to activate DC to DC, HIGH when on. Not currently used ***

// ****** Master Teensy Commuication ******
// Outputs the current state of the system to the master teensy
/*
typedef struct{
  uint8_t Identifier;
  uint16_t Voltage;
  uint16_t Current;
  uint8_t Thermistor_Temp;
  uint8_t LiPo_Charger;
  uint8_t S_DOI_1;
  uint8_t S_DOI_2;
  uint8_t CELL_VOLTAGE;
  uint8_t DOR_3;
  uint8_t Check_Sum;
} BATT_FS_TEMP_DATA;
*/

BATT_FS_TEMP_DATA batt_fs_temp_data;

// States are either 0 or 1, representing the states being LOW or HIGH
// **************************************** Need to establish error states

// ****** SD Card Logging Varibles ******
File LogFile;
const int ChipSelect = BUILTIN_SDCARD;  // Chips select, built in SD card slot
unsigned long Time;                     // Stores current time for each log entry
unsigned long Log_Entry_Counter = 0;           // Counts number of logs

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

// ******************************************************************** Set pre-scaler
void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  MasterTeensySerialPort.begin(115200);  // Master serial interface begin, 115200 speed
  LinkPROSerialPort.begin(2400);        // LinkPRO serial interface begin, 2400 speed

  // ****** System Control Setup ******
  pinMode(LiPo_Charger_Input, INPUT);
  pinMode(S_DOI_1, INPUT);
  pinMode(S_DOI_2, INPUT);
  pinMode(CELL_VOLTAGE_LOW, INPUT);
  pinMode(DOR_3, OUTPUT);
  digitalWrite(DOR_3, LOW); // Sets output LOW, DC to DC 1 is assumeed to be working on startup

  
  // ****** SD Card Initialization ******
  Serial.print("Initializing SD card... ");
  if (!SD.begin(ChipSelect)) { 
    // Error, SD card not initializated!
    Serial.println("Failed!"); 
    return;
  }
  Serial.println("Done");

  // Open log file
  LogFile = SD.open("logfile.txt", FILE_WRITE);
  if(LogFile){
    Serial.print("Printing startup message to SD card log file... ");
    LogFile.println();
    LogFile.println();
    LogFile.println("****** New Log Start ******");
    LogFile.println();
    LogFile.println("Log Start ==>");
    LogFile.println();
    LogFile.close(); // Close file
    Serial.println("Done");
  }
  else{
    // Error, SD card log file not open!
    Serial.println("Error opening file!");
  }
  
  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON

  LinkPRO_Data.Identifier = 1;
  Thermistor_Data.Identifier = 2;
  batt_fs_temp_data.Identifier = BATT_FS_TEMP;
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
  Serial.println("*** LinkPRO Decode ***");
  // Voltage Decode
  LinkPRO_Data.Voltage = (LinkPRO_Voltage[4] << 14) + (LinkPRO_Voltage[5] << 7) + LinkPRO_Voltage[6];
  
  // Current Decode
  // Signed ********
  LinkPRO_Data.Current = (LinkPRO_Current[4] << 14) + (LinkPRO_Current[5] << 7) + LinkPRO_Current[6];
  
  // Amphours Decode
  // Signed ********
  LinkPRO_Data.Amphours = (LinkPRO_Amphours[4] << 14) + (LinkPRO_Amphours[5] << 7) + LinkPRO_Amphours[6];
  
  // State Of Charge Decode
  LinkPRO_Data.State_Of_Charge = (LinkPRO_State_Of_Charge[4] << 14) + (LinkPRO_State_Of_Charge[5] << 7) + LinkPRO_State_Of_Charge[6];
  // Time To Charge Decode
  LinkPRO_Data.Time_To_Charge = (LinkPRO_Time_To_Charge[4] << 14) + (LinkPRO_Time_To_Charge[5] << 7) + LinkPRO_Time_To_Charge[6];
  // Temperature Decode, sensor not used, always 200
  LinkPRO_Data.Temperature = (LinkPRO_Temperature[4] << 14) + (LinkPRO_Temperature[5] << 7) + LinkPRO_Temperature[6];
  // Monitor Status Decode
  LinkPRO_Data.Monitor_Status[0] = (LinkPRO_Monitor_Status[4] >> 4) & 1; // ChargedVoltage
  LinkPRO_Data.Monitor_Status[1] = (LinkPRO_Monitor_Status[4] >> 3) & 1; // ChargedCurrent
  LinkPRO_Data.Monitor_Status[2] = (LinkPRO_Monitor_Status[4] >> 0) & 1; // AlarmTest
  
  LinkPRO_Data.Monitor_Status[3] = (LinkPRO_Monitor_Status[5] >> 6) & 1; // BacklightTest
  LinkPRO_Data.Monitor_Status[4] = (LinkPRO_Monitor_Status[5] >> 5) & 1; // DisplayTest
  LinkPRO_Data.Monitor_Status[5] = (LinkPRO_Monitor_Status[5] >> 4) & 1; // NoTemperatureSensor
  LinkPRO_Data.Monitor_Status[6] = (LinkPRO_Monitor_Status[5] >> 3) & 1; // SetupMode
  LinkPRO_Data.Monitor_Status[7] = (LinkPRO_Monitor_Status[5] >> 2) & 1; // HistoryMode
  LinkPRO_Data.Monitor_Status[8] = (LinkPRO_Monitor_Status[5] >> 1) & 1; // SuperLock
  LinkPRO_Data.Monitor_Status[9] = (LinkPRO_Monitor_Status[5] >> 0) & 1; // OverVoltage

  LinkPRO_Data.Monitor_Status[10] = (LinkPRO_Monitor_Status[6] >> 6) & 1; // UnderVoltage
  LinkPRO_Data.Monitor_Status[11] = (LinkPRO_Monitor_Status[6] >> 5) & 1; // BatteryLow
  LinkPRO_Data.Monitor_Status[12] = (LinkPRO_Monitor_Status[6] >> 4) & 1; // BatteryFlat
  LinkPRO_Data.Monitor_Status[13] = (LinkPRO_Monitor_Status[6] >> 3) & 1; // BatteryFull
  LinkPRO_Data.Monitor_Status[14] = (LinkPRO_Monitor_Status[6] >> 2) & 1; // ChargeBattery
  LinkPRO_Data.Monitor_Status[15] = (LinkPRO_Monitor_Status[6] >> 1) & 1; // MonitorOutOfSync
  LinkPRO_Data.Monitor_Status[16] = (LinkPRO_Monitor_Status[6] >> 0) & 1; // MonitorReset
  
  /*
   * D1: 4 = ChargedVoltage - Voltage > Charged Voltage level
   * D1: 3 = ChargedCurrent - Current < ChargeCurrent level
   * D1: 0 = AlarmTest - Alarmswitch triggered due to sys-com command
   * 
   * D2: 6 = BacklightTest - Backlight switched on due to sys-ex command
   * D2: 5 = DisplayTest - Display test switched on due to sys-ex command
   * D2: 4 = NoTemperatureSensor - No temperature sensor detected by the battery monitor
   * D2: 3 = SetupMode - Battery Monitor is in setup-mode
   * D2: 2 = HistoryMode - Battery Monitor is in history mode
   * D2: 1 = SuperLock - Super-lock enabled
   * D2: 0 = OverVoltage - Overvoltage alarm triggered
   * 
   * D3: 6 = UnderVoltage - Undervoltage alarm triggered
   * D3: 5 = BatteryLow - The discharge floor is reached, battery needs to be charged
   * D3: 4 = BatteryFlat - Battery is fully discharged (0.0%)
   * D3: 3 = BatteryFull - Battery is fully charged (100.0%)
   * D3: 2 = ChargeBattery - Battery needs to be charged
   * D3: 1 = MonitorOutOfSync - Monitor is not in sync with battery, charge battery
   * D3: 0 = MonitorReset - Monitor has been reset due to a power-loss
   * 
   */
    
   // 'Other' missing, not needed. Message 0x68 does not appear in data sheet
 
  Serial.print("Voltage Decoded: ");
  Serial.println(LinkPRO_Data.Voltage);
  Serial.print("Current Decoded: ");
  Serial.println(LinkPRO_Data.Current);
  Serial.print("Amphours Decoded: ");
  Serial.println(LinkPRO_Data.Amphours);
  Serial.print("State Of Charge Decoded: ");
  Serial.println(LinkPRO_Data.State_Of_Charge);
  Serial.print("Current Time To Charge: ");
  Serial.println(LinkPRO_Data.Time_To_Charge);
  Serial.print("Temperature: ");
  Serial.println(LinkPRO_Data.Temperature);
  Serial.println("Monitor Status Message:");
  Serial.print("  Byte 1 = ");
  for(int x = 7; x >= 0; x--){
    if((LinkPRO_Monitor_Status[4] >> x) & 1){
      Serial.print("1");
    }
    else{
      Serial.print("0");
    }
  }
  Serial.println();
  Serial.print("  Byte 2 = ");
  for(int x = 7; x >= 0; x--){
    if((LinkPRO_Monitor_Status[5] >> x) & 1){
      Serial.print("1");
    }
    else{
      Serial.print("0");
    }
  }
  Serial.println();
  Serial.print("  Byte 3 = ");
  for(int x = 7; x >= 0; x--){
    if((LinkPRO_Monitor_Status[6] >> x) & 1){
      Serial.print("1");
    }
    else{
      Serial.print("0");
    }
  }
  Serial.println();
  Serial.println("Monitor Status Decoded: ");
  Serial.print("ChargedVoltage = ");
  Serial.println(LinkPRO_Data.Monitor_Status[0]);
  Serial.print("ChargedCurrent = ");
  Serial.println(LinkPRO_Data.Monitor_Status[1]);
  Serial.print("AlarmTest = ");
  Serial.println(LinkPRO_Data.Monitor_Status[2]);
  Serial.print("BacklightTest = ");
  Serial.println(LinkPRO_Data.Monitor_Status[3]);
  Serial.print("DisplayTest = ");
  Serial.println(LinkPRO_Data.Monitor_Status[4]);
  Serial.print("NoTemperatureSensor = ");
  Serial.println(LinkPRO_Data.Monitor_Status[5]);
  Serial.print("SetupMode = ");
  Serial.println(LinkPRO_Data.Monitor_Status[6]);
  Serial.print("HistoryMode = ");
  Serial.println(LinkPRO_Data.Monitor_Status[7]);
  Serial.print("SuperLock = ");
  Serial.println(LinkPRO_Data.Monitor_Status[8]);
  Serial.print("OverVoltage = ");
  Serial.println(LinkPRO_Data.Monitor_Status[9]);
  Serial.print("UnderVoltage = ");
  Serial.println(LinkPRO_Data.Monitor_Status[10]);
  Serial.print("BatteryLow = ");
  Serial.println(LinkPRO_Data.Monitor_Status[11]);
  Serial.print("BatteryFlat = ");
  Serial.println(LinkPRO_Data.Monitor_Status[12]);
  Serial.print("BatteryFull = ");
  Serial.println(LinkPRO_Data.Monitor_Status[13]);
  Serial.print("ChargeBattery = ");
  Serial.println(LinkPRO_Data.Monitor_Status[14]);
  Serial.print("MonitorOutOfSync = ");
  Serial.println(LinkPRO_Data.Monitor_Status[15]);
  Serial.print("MonitorReset = ");
  Serial.println(LinkPRO_Data.Monitor_Status[16]);  
  Serial.println();

  // Populate BATT_FS_TEMP_DATA structure with the current decoded values
  batt_fs_temp_data.Voltage = LinkPRO_Data.Voltage;
  batt_fs_temp_data.Current = LinkPRO_Data.Current;
 
}

// Calculate Average Thermistor Reading
void Average_Thermistor_Readings(){
  Serial.print("Averaging thermistor readings... ");
  
  // Calculate Average thermistor reading
  Average_Thermistor_Reading_1 = Thermistor_Samples_1 / SampleCounter;
  Average_Thermistor_Reading_2 = Thermistor_Samples_2 / SampleCounter;
  Average_Thermistor_Reading_3 = Thermistor_Samples_3 / SampleCounter;
  Average_Thermistor_Reading_4 = Thermistor_Samples_4 / SampleCounter;
  
  /*
  Serial.print("Thermistor 1 averaged reading: ");
  Serial.println(Average_Thermistor_Reading_1);
  Serial.print("Thermistor 2 averaged reading: ");
  Serial.println(Average_Thermistor_Reading_2);
  Serial.print("Thermistor 3 averaged reading: ");
  Serial.println(Average_Thermistor_Reading_3);
  Serial.print("Thermistor 4 averaged reading: ");
  Serial.println(Average_Thermistor_Reading_4);
  */
  
  // Convert the analog reading resistance to a resistance
  Average_Thermistor_Reading_1 = (1023/Average_Thermistor_Reading_1) - 1;
  Average_Thermistor_Reading_2 = (1023/Average_Thermistor_Reading_2) - 1;
  Average_Thermistor_Reading_3 = (1023/Average_Thermistor_Reading_3) - 1;
  Average_Thermistor_Reading_4 = (1023/Average_Thermistor_Reading_4) - 1;

  Thermistor_Data.Thermistor1 = Average_Thermistor_Reading_1;
  Thermistor_Data.Thermistor2 = Average_Thermistor_Reading_2;
  Thermistor_Data.Thermistor3 = Average_Thermistor_Reading_3;
  Thermistor_Data.Thermistor4 = Average_Thermistor_Reading_4;

  /*
  Serial.print("Thermistor 1 resistance value: ");
  Serial.println(Thermistor_Data.Thermistor1);
  Serial.print("Thermistor 2 resistance value: ");
  Serial.println(Thermistor_Data.Thermistor2);
  Serial.print("Thermistor 3 resistance value: ");
  Serial.println(Thermistor_Data.Thermistor3);
  Serial.print("Thermistor 4 resistance value: ");
  Serial.println(Thermistor_Data.Thermistor4);
  */
  SampleCounter = 0;

  // Add theshold value for battery temperature warning ******************
  batt_fs_temp_data.Thermistor_Temp = 1;

  Serial.println("Done");
}

// Send Data to Master Teensy
void Send_Master_Data(){
  // ***************** Add check sum

  Serial.print("Sending data to Master Teensy... ");
  
  /*
  // Sample data for debug
  BATT_FS_TEMP_DATA.Identifier = BATT_FS_TEMP;
  BATT_FS_TEMP_DATA.Voltage = 1143;
  BATT_FS_TEMP_DATA.Current = 6;
  BATT_FS_TEMP_DATA.Thermistor_Temp = 0;
  BATT_FS_TEMP_DATA.LiPo_Charger = 0;
  BATT_FS_TEMP_DATA.S_DOI_1 = 1;
  BATT_FS_TEMP_DATA.S_DOI_2 = 0;
  BATT_FS_TEMP_DATA.CELL_VOLTAGE = 0;
  BATT_FS_TEMP_DATA.DOR_3 = 0;
  BATT_FS_TEMP_DATA.Check_Sum = 0;

  */

  batt_fs_temp_data.S_DOI_1 = 1;
  
  char Buffer[sizeof(batt_fs_temp_data) + 1];
  memcpy(Buffer, &batt_fs_temp_data, sizeof(batt_fs_temp_data));
  MasterTeensySerialPort.write(Buffer, (sizeof(batt_fs_temp_data)));
  MasterTeensySerialPort.write('t');
  
  /*
  // Print data manually to serial port
  MasterTeensySerialPort.println(Master_Data.Identifier);
  MasterTeensySerialPort.println(Master_Data.Voltage);
  MasterTeensySerialPort.println(Master_Data.Current);
  MasterTeensySerialPort.println(Master_Data.Thermistor_Temp);
  MasterTeensySerialPort.println(Master_Data.LiPo_Charger);
  MasterTeensySerialPort.println(Master_Data.S_DOI_1);
  MasterTeensySerialPort.println(Master_Data.S_DOI_2);
  MasterTeensySerialPort.println(Master_Data.CELL_VOLTAGE);
  MasterTeensySerialPort.println(Master_Data.DOR_3);
  MasterTeensySerialPort.println(Master_Data.Check_Sum);
  */
  Serial.println("Done");
}

void Print_SD_Log_Entry(){
  Serial.print("Writing to SD card... ");
  // Open file
  LogFile = SD.open("logfile.txt", FILE_WRITE);
  Time = millis();  // Record current time, for log entry
     
  // If the file is open, write start up message
  if(LogFile){
    Log_Entry_Counter++; // Increment log counter
    LogFile.print("Log: ");
    LogFile.println(Log_Entry_Counter);
    LogFile.print("Current time = ");
    LogFile.println(Time);
    LogFile.println();
    // Log current LinkPRO data
    LogFile.println("LinkPRO data:");
    LogFile.print("Voltage = ");
    LogFile.println(LinkPRO_Data.Voltage);
    LogFile.print("Current = ");
    LogFile.println(LinkPRO_Data.Current);
    LogFile.print("Amphours = ");
    LogFile.println(LinkPRO_Data.Amphours);
    LogFile.print("State Of Charged = ");
    LogFile.println(LinkPRO_Data.State_Of_Charge);
    LogFile.print("Current Time To Charge: ");
    LogFile.println(LinkPRO_Data.Time_To_Charge);
    LogFile.println();
    LogFile.println("Monitor Status:");
    LogFile.print("ChargedVoltage = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[0]);
    LogFile.print("ChargedCurrent = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[1]);
    LogFile.print("AlarmTest = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[2]);
    LogFile.print("BacklightTest = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[3]);
    LogFile.print("DisplayTest = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[4]);
    LogFile.print("NoTemperatureSensor = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[5]);
    LogFile.print("SetupMode = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[6]);
    LogFile.print("HistoryMode = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[7]);
    LogFile.print("SuperLock = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[8]);
    LogFile.print("OverVoltage = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[9]);
    LogFile.print("UnderVoltage = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[10]);
    LogFile.print("BatteryLow = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[11]);
    LogFile.print("BatteryFlat = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[12]);
    LogFile.print("BatteryFull = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[13]);
    LogFile.print("ChargeBattery = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[14]);
    LogFile.print("MonitorOutOfSync = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[15]);
    LogFile.print("MonitorReset = ");
    LogFile.println(LinkPRO_Data.Monitor_Status[16]);  
    LogFile.println();
    // Log current thermistor data
    LogFile.println("Thermistor data:");
    LogFile.print("Thermistor 1 resistance value = ");
    LogFile.println(Thermistor_Data.Thermistor1);
    LogFile.print("Thermistor 2 resistance value = ");
    LogFile.println(Thermistor_Data.Thermistor2);
    LogFile.print("Thermistor 3 resistance value = ");
    LogFile.println(Thermistor_Data.Thermistor3);
    LogFile.print("Thermistor 4 resistance value = ");
    LogFile.println(Thermistor_Data.Thermistor4);
    LogFile.println();
    // Log message sent to Master Teensy
    LogFile.println("Message sent to Master Teensy:");
    LogFile.print("Identifier = ");
    LogFile.println(batt_fs_temp_data.Identifier);
    LogFile.print("Voltage = ");
    LogFile.println(batt_fs_temp_data.Voltage);
    LogFile.print("Current = ");
    LogFile.println(batt_fs_temp_data.Current);
    LogFile.print("Thermistor_Temp = ");
    LogFile.println(batt_fs_temp_data.Thermistor_Temp);
    LogFile.print("LiPo_Charger = ");
    LogFile.println(batt_fs_temp_data.LiPo_Charger);
    LogFile.print("S-DOI-1 = ");
    LogFile.println(batt_fs_temp_data.S_DOI_1);
    LogFile.print("S-DOI-2 = ");
    LogFile.println(batt_fs_temp_data.S_DOI_2);
    LogFile.print("CELL_VOLTAGE: ");
    LogFile.println(batt_fs_temp_data.CELL_VOLTAGE);
    LogFile.print("DOR-3 = ");
    LogFile.println(batt_fs_temp_data.DOR_3);
    LogFile.print("Check_Sum = ");
    LogFile.println(batt_fs_temp_data.Check_Sum);
    LogFile.println();
    // End of log, entry seperator
    LogFile.println("************************************************");
    LogFile.println();
    LogFile.close(); // Close file
    Serial.println("Done");
  }
  else{
    // The file is not open log error ******
    Serial.println("Error opening file!");
  }
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
        Message_Counter++;
      }
     
      LinkProBuffer[ByteCounter] = IncomingSerialData;
      //Serial.print(LinkProBuffer[ByteCounter], HEX);
      
      // Look for end of message
      if(IncomingSerialData == 0xFF){
        Serial.println();
        // Check the message type
        //Message_Counter++;
        if(LinkProBuffer[3] == 0x60){
          Message_Counter = 0;
          Serial.print("Voltage Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Voltage[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Voltage[i], HEX);
          }     
        }
        else if(LinkProBuffer[3] == 0x61){
          Serial.print("Current Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Current[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Current[i], HEX);
          }
        }
        else if(LinkProBuffer[3] == 0x62){
          Serial.print("Amphours Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Amphours[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Amphours[i], HEX);
          }
        }
        else if(LinkProBuffer[3] == 0x64){
          Serial.print("State Of Change Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_State_Of_Charge[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_State_Of_Charge[i], HEX);
          }
        }
        else if(LinkProBuffer[3] == 0x65){
          Serial.print("Time To Charge Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Time_To_Charge[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Time_To_Charge[i], HEX);
          }
        }
        else if(LinkProBuffer[3] == 0x66){
          Serial.print("Temperature Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Temperature[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Temperature[i], HEX);
          }
        }
        else if(LinkProBuffer[3] == 0x67){
          Serial.print("Monitor Status Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Monitor_Status[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Monitor_Status[i], HEX);
          }
        }
        else if(LinkProBuffer[3] == 0x68){
          Serial.print("Other Message: ");
          for(int i = 0; i <= 7; i++){
            LinkPRO_Other[i] = LinkProBuffer[i];
            Serial.print(LinkPRO_Other[i], HEX);
          }
          LinkPRO_Complete = true;
        }
        else{
          Serial.print(" <-- Error: Wrong message ID");  // ****** Error Minor ******
        }

        Serial.println();
        if(LinkPRO_Complete == true && Message_Counter == 7){
          Decode_LinkPRO_Data();
          Average_Thermistor_Readings();
          Send_Master_Data();
          Print_SD_Log_Entry();
          LinkPRO_Complete = false;
          Serial.println();
          Serial.println("************************************************");
          Serial.println();
          Serial.println("*** LinkPRO Message ***");
        }
      }
    }
  }

  // Reads state of RC charger
  if(digitalRead(LiPo_Charger_Input) == LOW){
    // Error, LiPo not being charged!
    // Systems must be aware of limited system battery life
    batt_fs_temp_data.LiPo_Charger = 0;
  }
  else{
    // Working correctly
    batt_fs_temp_data.LiPo_Charger = 1;
  }
  // Read current state of S-DOI-1
  if(digitalRead(S_DOI_1) == LOW){
    // Error, DC to DC not working!
    batt_fs_temp_data.S_DOI_1 = 0;   
    // If DC to DC 2 is not turned ON, turn it ON
    if(batt_fs_temp_data.DOR_3 == 0){
      digitalWrite(DOR_3, HIGH); // Sets output HIGH, turning DC to DC 2 ON
      batt_fs_temp_data.DOR_3 = 1;
    }
  }
  else{
    // Working correctly
    batt_fs_temp_data.S_DOI_1 = 1;
    // If DC to DC 2 is ON, turn it OFF. Both are not needed
    if(batt_fs_temp_data.DOR_3 == 1){
      digitalWrite(DOR_3, LOW); // Sets output LOW, turning DC to DC 2 OFF
      batt_fs_temp_data.DOR_3 = 0;
    }
  }

  //Read current state of S-DOI-2
  if(digitalRead(S_DOI_2) == LOW){
    batt_fs_temp_data.S_DOI_1 = 0;
  }
  else{
    batt_fs_temp_data.S_DOI_1 = 1;
  }

  // Read current state of CELL_VOLTAGE_LOW
  if(digitalRead(CELL_VOLTAGE_LOW) == HIGH){
    // Error, indicating low cell voltage!
    batt_fs_temp_data.CELL_VOLTAGE = 1;
  }
  else{
    // Cell voltage about threshold
    batt_fs_temp_data.CELL_VOLTAGE = 0;
  }

  // Read each thermistor and stores it in the arrays
  SampleCounter++;
  Thermistor_Samples_1 = Thermistor_Samples_1 + analogRead(Thermistor1);
  Thermistor_Samples_2 = Thermistor_Samples_2 + analogRead(Thermistor2);
  Thermistor_Samples_3 = Thermistor_Samples_3 + analogRead(Thermistor3);
  Thermistor_Samples_4 = Thermistor_Samples_4 + analogRead(Thermistor4);

}



