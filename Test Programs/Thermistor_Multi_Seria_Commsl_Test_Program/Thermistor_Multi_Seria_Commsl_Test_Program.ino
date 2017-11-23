// ****** Serial Communication Initialization ******
// Define serial ports used
#define SerialPort1 Serial1

// Serial communication varibles
char IncomingSerialData;  // Stores the current ACSII value from the serial port
char Buffer[64];  // This is used to store the current message
int MessageCounter = 0;  // This is the counter used to read the ASCII characters from the serial port
boolean MessageRecieved;  // Used to indicates when there is a message
boolean MessageComplete;  // Used to indicates when the message has finished
int MessageCommaPosition[64]; // Stores the positions of the commas in the message
int NumberOfCommas = 0; // Counts the number of commas in the message
int NumberOfDataValues; // Number of comma separated values in the message, number of commas + 1

// ****** Thermistor Varibles ******
#define ThermistorSeriesResistorValue 10000 // Define value of thermistor series resistor

// Define thermistor pins
#define Thermistor1 A0
#define Thermistor2 A1
#define Thermistor3 A2
#define Thermistor4 A3

float ThermistorReading[4]; // Array of thermistor readings

// Define sample varibles
const int NumberofThermistorSamples = 5;
// Sample array for each thermistor
int ThermistorSamples[NumberofThermistorSamples * 4];  // Number of sample x number of thermistors
float AverageThermistorValue; // Stores averaged thermistor reading
int SampleCounter;
int ThermistorCounter;
int ThermistorTime = 0;

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  SerialPort1.begin(9600);  // Teensy serial interface begin, default speed

  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON

}

void ThermistorRead(){
  // Collect sample of thermistor readings
  for(SampleCounter = 0; SampleCounter < NumberofThermistorSamples; SampleCounter++){
    // Read each thermistor and stores it in a array, offset by the number of samples
    ThermistorSamples[SampleCounter] = analogRead(Thermistor1);
    ThermistorSamples[SampleCounter + NumberofThermistorSamples] = analogRead(Thermistor2);
    ThermistorSamples[SampleCounter + (NumberofThermistorSamples * 2)] = analogRead(Thermistor3);
    ThermistorSamples[SampleCounter + (NumberofThermistorSamples * 3)] = analogRead(Thermistor4);

    // ****** Debug ******
    /*
    Serial.println(ThermistorSamples[SampleCounter]);
    Serial.println(ThermistorSamples[SampleCounter + NumberofThermistorSamples]);
    Serial.println(ThermistorSamples[SampleCounter + (NumberofThermistorSamples * 2)]);
    Serial.println(ThermistorSamples[SampleCounter + (NumberofThermistorSamples * 3)]);
    */
  }

  // Calculate Average thermistor reading
  for(ThermistorCounter = 0; ThermistorCounter < 4; ThermistorCounter++){
    AverageThermistorValue = 0;
    for(SampleCounter = 0; SampleCounter < NumberofThermistorSamples; SampleCounter++){
      AverageThermistorValue += ThermistorSamples[ThermistorCounter + (NumberofThermistorSamples * (ThermistorCounter))];
    }
    AverageThermistorValue = AverageThermistorValue / NumberofThermistorSamples;

    // ****** Debug ******
    /*
    Serial.print("Thermistor: ");
    Serial.print(ThermistorCounter + 1);
    Serial.print(" averaged reading: ");
    Serial.println(AverageThermistorValue);
    */
    
    // Convert the analog reading resistance to a recistance
    AverageThermistorValue = (1023/AverageThermistorValue) - 1;
    ThermistorReading[ThermistorCounter] = ThermistorSeriesResistorValue / AverageThermistorValue;
    
    Serial.print("Thermistor: ");
    Serial.print(ThermistorCounter + 1);
    Serial.print(" resistance: "); 
    Serial.println(ThermistorReading[ThermistorCounter]);
  }
}

void loop() {
  // Every two five seconds the system checks the thermistor values and outputs to master teensy
  // TEST ONLY, I think in the final version the master teensy will only be informed if there is a problem ************
  // This could be changes so the program is constantly taking reading and averages every 5 seconds ******************
  if(millis() - ThermistorTime  >= 5000){
    ThermistorRead();
    String OutgoingSerialData;
    OutgoingSerialData = "*B,T,";
    OutgoingSerialData += ThermistorReading[0];
    OutgoingSerialData += ",";
    OutgoingSerialData += ThermistorReading[1];
    OutgoingSerialData += ",";
    OutgoingSerialData += ThermistorReading[2];
    OutgoingSerialData += ",";
    OutgoingSerialData += ThermistorReading[3];
    OutgoingSerialData += "#";
    //OutgoingSerialData = String("*B,T," + 'ThermistorReading[1]' + "," + 'ThermistorReading[2]' + "," + 'ThermistorReading[3]' + "," + 'ThermistorReading[4]' + "#"));
    Serial.println(OutgoingSerialData);
    SerialPort1.println(OutgoingSerialData);
        
    ThermistorTime= millis();
  }

  
  // If there is data available on the serial port the program reads it in
  if(SerialPort1.available() > 0){
    //Serial.print("Incoming "); // Display to user for debug ***
    while(SerialPort1.available()){
      
      // Read data from serial port
      IncomingSerialData = Serial1.read(); // Reads the current ASCII value in the data record
      Serial.print("Character: "); // Display to user for debug ***
      Serial.println(String(IncomingSerialData)); // Display to user for debug ***

      // Look the start of the message * which is ASCII character value 42
      if(IncomingSerialData == 42){
        //Serial.println("Message Start"); // Display to user for debug ***
        MessageRecieved = true;  // Indicates there is a message
        MessageCounter = 0; // Resets message counter
        NumberOfCommas = 0; // Resets comma counter
        Buffer[0]= IncomingSerialData;
        // For comma seprated values use, * counts as a comma
        NumberOfCommas++;
        MessageCommaPosition[NumberOfCommas] = MessageCounter;  // Record location of comma in message    
      } 
      else{
        if(MessageRecieved == true){
          MessageCounter++;
          Buffer[MessageCounter] = IncomingSerialData;
          // Look the end of the message # which is ASCII character value 35
          if(IncomingSerialData == 35){
            Serial.println("Message End"); // Display to user for debug ***
            MessageRecieved = false;  // Indicates there is no message, as it has finished
            MessageComplete = true;  // Indicates the message has finished
            // For comma seprated values use, # counts as a comma
            NumberOfCommas++;
            MessageCommaPosition[NumberOfCommas] = MessageCounter;  // Record location of comma in message    
          }
          // Look the comma in the message , which is ASCII character value 44
          else if(IncomingSerialData == 44){
            //Serial.println("Comma"); // Display to user for debug ***
            NumberOfCommas++;
            MessageCommaPosition[NumberOfCommas] = MessageCounter;  // Record location of comma in message
          }
        }
      }    
    }
  }
  // When the message is complete, the result is printed to debug and the data is parsed up and stored in varibles
  //****** This could go in above in message end detection section? ****** Might be more clear if seperate?
  if(MessageComplete == true){
    //Serial.print("Message Counter: ");
    //Serial.println(MessageCounter);  // Display to user for debug ***
    for(int PrintCounter = 0; PrintCounter <= MessageCounter; PrintCounter++){      
      Serial.print(Buffer[PrintCounter]);
    }
    Serial.println();

    Serial.println("*** Value Seperation Debug ***");
    Serial.print("Number of Comma's: ");
    Serial.println(NumberOfCommas);

    // ****** Comma separated value separation ******
    NumberOfDataValues = NumberOfCommas - 1;  // Number of Data Chunks in the number of comma's - 1
    String DataValues[NumberOfDataValues]; // Creates 2D array to store the datachunks

    Serial.print("Number of Data Values: ");
    Serial.println(NumberOfDataValues);

    for(int DataValuesCounter = 1; DataValuesCounter <= NumberOfDataValues; DataValuesCounter++){
      Serial.print("Value ");
      Serial.print(DataValuesCounter);
      Serial.print(" = ");

      for(int CommaCounter = MessageCommaPosition[DataValuesCounter] + 1; CommaCounter < MessageCommaPosition[DataValuesCounter + 1]; CommaCounter++){
        Serial.print(Buffer[CommaCounter]);
        if(Buffer[CommaCounter] != 44 && Buffer[CommaCounter] != 35){
          DataValues[DataValuesCounter] = DataValues[DataValuesCounter] + Buffer[CommaCounter];
        }
      }
      Serial.println();
    }
    
    for(int DataValuesPrintCounter = 1; DataValuesPrintCounter <= NumberOfDataValues; DataValuesPrintCounter++){
      Serial.print("Value Stored: ");
      Serial.print(DataValuesPrintCounter);
      Serial.print(" = ");
      Serial.println(DataValues[DataValuesPrintCounter]);
    }

    /*
    Serial.println(DataValues[1]);
    Serial.println(DataValues[2]);
    Serial.println(DataValues[3]);
    Serial.println(DataValues[4]);
    */
    
    MessageComplete = false;  // Set the message complete flag to false
  }
}
