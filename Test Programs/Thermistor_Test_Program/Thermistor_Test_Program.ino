// ****** Thermistor Varibles ******
#define ThermistorSeriesResistorValue 10000 // Define value of thermistor series resistor

// Define thermistor pins
#define Thermistor1 A0

// Define thermistor reading varibles
float ThermistorReading1;

// Define sample varibles
const int NumberofThermistorSamples = 5;
int ThermistorSamples[NumberofThermistorSamples];
float AverageThermistorValue;
int ThermistorCounter;

// Steinhart estimate varibles
float Steinhart;
#define ThermistorNominal 10000
#define TemperatureNominal 25
#define BCOEfficient 3380 // From datasheet


void setup() {
  // ******Serial Interface Setup ******
  Serial.begin(9600); // Serial interface begin, default speed

}

void loop() {
  // Collect sample of thermistor readings
  ThermistorCounter = 0;
  for(ThermistorCounter = 0; ThermistorCounter < NumberofThermistorSamples; ThermistorCounter++){
    ThermistorSamples[ThermistorCounter] = analogRead(Thermistor1);
    Serial.println(ThermistorSamples[ThermistorCounter]);
    delay(10);
  }

  // Calculate Average thermistor reading
  ThermistorCounter = 0;
  AverageThermistorValue = 0;
  for(ThermistorCounter = 0; ThermistorCounter < NumberofThermistorSamples; ThermistorCounter++){
    AverageThermistorValue += ThermistorSamples[ThermistorCounter];
  }

  AverageThermistorValue = AverageThermistorValue / NumberofThermistorSamples;

  Serial.print("Averaged Reading ");
  Serial.println(AverageThermistorValue);

  // Convert the analog reading resistance to a recistance
  AverageThermistorValue = (1023/AverageThermistorValue) - 1;
  AverageThermistorValue = ThermistorSeriesResistorValue / AverageThermistorValue;

  Serial.print("Thermistor resistance "); 
  Serial.println(AverageThermistorValue);

  // Convert Resistance to temperature
  // Steinhart-Hart equation
  // This is just an estimate for testing purposes, normally a resitance table is needed
  // In practise we can hard code in temperature ranges, i.e. if temp is above "value" then do "x"
  Steinhart = AverageThermistorValue / ThermistorNominal;
  Steinhart = log(Steinhart);
  Steinhart /= BCOEfficient;
  Steinhart +=1.0 / (TemperatureNominal + 273.15);
  Steinhart = 1.0 / Steinhart;
  Steinhart -= 273.15;

  Serial.print("Temperature "); 
  Serial.print(Steinhart);
  Serial.println(" *C");

  delay(1000);
}
