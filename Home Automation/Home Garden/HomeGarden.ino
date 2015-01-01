#include <Sensirion.h>

const int Relay = 7;
const uint8_t dataPin =  9;              // SHT serial data
const uint8_t sclkPin =  8;              // SHT serial clock
const uint8_t ledPin  = 13;              // Arduino built-in LED
const uint32_t TRHSTEP   = 5000UL;       // Sensor query period
const uint32_t BLINKSTEP =  250UL;       // LED blink period
 
Sensirion sht = Sensirion(dataPin, sclkPin);
 
uint16_t rawData;
float temperature;
float humidity;
float dewpoint;
 
byte measActive = false;
byte measType = TEMP;
 
unsigned long trhMillis = 0;             // Time interval tracking
unsigned long blinkMillis = 0;
 
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(Relay, OUTPUT);
  delay(15);                             // Wait >= 11 ms before first cmd
// Demonstrate blocking calls
  sht.measTemp(&rawData);                // sht.meas(TEMP, &rawData, BLOCK)
  temperature = sht.calcTemp(rawData);
  sht.measHumi(&rawData);                // sht.meas(HUMI, &rawData, BLOCK)
  humidity = sht.calcHumi(rawData, temperature);
  dewpoint = sht.calcDewpoint(humidity, temperature);
  logData();
}
 
void loop() {
  unsigned long curMillis = millis();          // Get current time
 
  // Rapidly blink LED.  Blocking calls take too long to allow this.
  if (curMillis - blinkMillis >= BLINKSTEP) {  // Time to toggle the LED state?
    ledState ^= 1;
    digitalWrite(ledPin, ledState);
    blinkMillis = curMillis;
  }
 
  // Demonstrate non-blocking calls
  if (curMillis - trhMillis >= TRHSTEP) {      // Time for new measurements?
    measActive = true;
    measType = TEMP;
    sht.meas(TEMP, &rawData, NONBLOCK);        // Start temp measurement
    trhMillis = curMillis;
  }
  if (measActive && sht.measRdy()) {           // Note: no error checking
    if (measType == TEMP) {                    // Process temp or humi?
      measType = HUMI;
      temperature = sht.calcTemp(rawData);     // Convert raw sensor data
      sht.meas(HUMI, &rawData, NONBLOCK);      // Start humidity measurement
    } else {
      measActive = false;
      humidity = sht.calcHumi(rawData, temperature); // Convert raw sensor data
      dewpoint = sht.calcDewpoint(humidity, temperature);
      logData();
      Water();
    }
  }
}
 
void logData() {
  Serial.print("Temperature = ");   Serial.print(temperature);
  Serial.print(" C, Humidity = ");  Serial.print(humidity);
  Serial.print(" %, Dewpoint = ");  Serial.print(dewpoint);
  Serial.println(" C");
}

void Water() {
 if (humidity < 70.0) {
  digitalWrite(Relay, HIGH);
  Serial.println("Relay ON");
  delay(2000);
  digitalWrite(Relay, LOW);
  Serial.println("Relay OFF");
 } else {
  digitalWrite(Relay, HIGH);
  Serial.println("Relay OFF");
 }
 delay(2000);
}
