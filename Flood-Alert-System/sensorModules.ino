//pin which triggers ultrasonic sound
const int pingPin = 13;
 
//range in cm which is considered safe to enter, anything
//coming within less than 5 cm triggers red LED
int safeZone = 5; 

void setup() {
  // initialize serial communication
  Serial.begin(9600);
}
 
void loop()
{
  //raw duration in milliseconds, cm is the
  //converted amount into a distance
  long duration, cm;
 
  //initializing the pin states
  pinMode(pingPin, OUTPUT); 
 
  //sending the signal, starting with LOW for a clean signal
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
 
  //setting up the input pin, and receiving the duration in
  //microseconds for the sound to bounce off the object infront
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
 
  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  Serial.write(cm);
  delay(1000);
}
 
long microsecondsToCentimeters(long microseconds)
{ 
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
