// ****** Serial Communication Define ******
// Define serial ports used
#define SerialPort1 Serial1

// Serial communication varibles
char IncomingSerialData;  // Stores the current ACSII value from the serial port
char Buffer[64];  // This is used to store the current message
int MessageCounter = 0;  // This is the counter used to read the ASCII characters from the serial port
boolean MessageRecieved;  // Used to indicates when there is a message
boolean MessageComplete;  // Used to indicates when the message has finished
int MessageCommaPosition[10]; // Stores the positions of the commas in the message
int NumberOfCommas = 0; // Counts the number of commas in the message

boolean TestStarted = false;

const int ledPin = 13;

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed
  SerialPort1.begin(9600);  // Teensy serial interface begin, default speed

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);   // set the LED on
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
        MessageComplete = false;  // Set the message complete flag to false
        MessageCounter = 0; // Resets message counter
        NumberOfCommas = 0; // Resets comma counter
        Buffer[0]= IncomingSerialData;
      }
      else{
        if(MessageRecieved == true){
          MessageCounter++;
          Buffer[MessageCounter] = IncomingSerialData;
          // Look the end of the message # which is ASCII character value 35
          if(IncomingSerialData == 35){
            Serial.println("Message End"); // Display to user for debug ***
            MessageRecieved = false;  // Indicates there is no message
            MessageComplete = true;  // Indicates the message has finished    
          }
          // Look the comma in the message , which is ASCII character value 44
          else if(IncomingSerialData == 44){
            //Serial.println("Comma"); // Display to user for debug ***
            NumberOfCommas++;
          }
        }
      }    
    }
  }
  // When the message is complete, the result is printed to debug
  if(MessageComplete == true){
    //Serial.print("Message Counter: ");
    //Serial.println(MessageCounter);
    for(int PrintCounter = 0; PrintCounter <= MessageCounter; PrintCounter++){      
      Serial.print(Buffer[PrintCounter]);
    }
    Serial.println();
    MessageComplete = false;
  }
}
