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

void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // Serial interface begin, default speed

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
    Serial.println(ThermistorSamples[SampleCounter]);
    Serial.println(ThermistorSamples[SampleCounter + NumberofThermistorSamples]);
    Serial.println(ThermistorSamples[SampleCounter + (NumberofThermistorSamples * 2)]);
    Serial.println(ThermistorSamples[SampleCounter + (NumberofThermistorSamples * 3)]);
  }

  // Calculate Average thermistor reading
  for(ThermistorCounter = 0; ThermistorCounter < 4; ThermistorCounter++){
    AverageThermistorValue = 0;
    for(SampleCounter = 0; SampleCounter < NumberofThermistorSamples; SampleCounter++){
      AverageThermistorValue += ThermistorSamples[ThermistorCounter + (NumberofThermistorSamples * (ThermistorCounter))];
    }
    AverageThermistorValue = AverageThermistorValue / NumberofThermistorSamples;

    // ****** Debug ******
    Serial.print("Thermistor: ");
    Serial.print(ThermistorCounter + 1);
    Serial.print(" averaged reading: ");
    Serial.println(AverageThermistorValue);
    
    // Convert the analog reading resistance to a recistance
    AverageThermistorValue = (1023/AverageThermistorValue) - 1;
    ThermistorReading[ThermistorCounter] = ThermistorSeriesResistorValue / AverageThermistorValue;
    
    Serial.print("Thermistor: ");
    Serial.print(ThermistorCounter + 1);
    Serial.print(" resistance: "); 
    Serial.println(ThermistorReading[ThermistorCounter]);
  }
}

void loop(){
  ThermistorRead();
  delay(1000);

  // Output to serial for master teensy, when requested?
}
