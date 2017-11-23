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

void loop() {
  //Serial.println("Loop"); // Display to user for debug ***
  
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
          //Buffer[CommaCounter].trim();
          DataValues[DataValuesCounter] = DataValues[DataValuesCounter] + Buffer[CommaCounter];
          Serial.print("Loop");
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
