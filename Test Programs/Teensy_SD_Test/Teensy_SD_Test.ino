// Libararies used for SD card storage
#include <SD.h>
#include <SPI.h> 

File MyFile;

//****** Power LED Initialization ******
const int PowerLEDPin = 13;  // Defines on-board LED pin

const int ChipSelect = BUILTIN_SDCARD;

unsigned long Time;

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // PC serial interface begin, default speed

  // ****** Power LED Setup ******
  pinMode(PowerLEDPin , OUTPUT);
  digitalWrite(PowerLEDPin , HIGH);   // Set the on-board LED high, to indicate the teensy is ON

  // ****** SD Card Initialization ******
  Serial.print("Initializing SD card...");
  if (!SD.begin(ChipSelect)) { 
    Serial.println("initialization failed!"); 
    return; 
  }
  Serial.println("Initialization done");

  // Open file, you can only have one file open at a time
  MyFile = SD.open("test.txt", FILE_WRITE);

  // If the file is open, write start up message
  if(MyFile){
    Serial.print("Writing start up message to SD card");
    MyFile.println("SD card test start up message");
    MyFile.close(); // Close file
    Serial.println("Done");
  }
  else{
    // The file is not open log error ******
    Serial.println("Error opening MyFile");
  }
}

void loop() {
  delay(2000);
  
  // Open file
  MyFile = SD.open("test.txt", FILE_WRITE);

  Time = millis();  // Record current time
  
  // If the file is open, write start up message
  if(MyFile){
    Serial.print("Writing to SD card: ");
    MyFile.print("Current time = ");
    MyFile.println(Time);
    MyFile.close(); // Close file
    Serial.println("Done");
  }
  else{
    // The file is not open log error ******
    Serial.println("Error opening MyFile");
  }
  

}
