const int pingPin = 7;
const int pingPin2 = 8;
const int pingPin3 = 12;
const int pingPin4 = 13;
unsigned int duration, inches;
unsigned int duration2, inches2;
unsigned int duration3, inches3;
unsigned int duration4, inches4;
#include<Servo.h>

Servo myservo;
Servo myservo2;
void setup() {
  Serial.begin(9600);
   myservo.attach(3);
   myservo2.attach(5);
}

void loop() {
  pinMode(pingPin, OUTPUT);          // Set pin to OUTPUT
  digitalWrite(pingPin, LOW);        // Ensure pin is low
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);       // Start ranging
  delayMicroseconds(5);              //   with 5 microsecond burst
  digitalWrite(pingPin, LOW);        // End ranging
  pinMode(pingPin, INPUT);           // Set pin to INPUT
  duration = pulseIn(pingPin, HIGH);  // Read echo pulse
 
 pinMode(pingPin2, OUTPUT);        
  digitalWrite(pingPin2, LOW);       
  delayMicroseconds(2);
  digitalWrite(pingPin2, HIGH);       
  delayMicroseconds(5);            
  digitalWrite(pingPin2, LOW);        
  pinMode(pingPin2, INPUT);     
 
   duration2 = pulseIn(pingPin2, HIGH); 
   
   pinMode(pingPin3, OUTPUT);       
  digitalWrite(pingPin3, LOW);       
  delayMicroseconds(2);
  digitalWrite(pingPin3, HIGH);       
  delayMicroseconds(5);              
  digitalWrite(pingPin3, LOW);      
  pinMode(pingPin3, INPUT);     
  
  duration3 = pulseIn(pingPin3, HIGH);
  
   pinMode(pingPin4, OUTPUT);         
  digitalWrite(pingPin4, LOW);     
  delayMicroseconds(2);
  digitalWrite(pingPin4, HIGH);       
  delayMicroseconds(5);              
  digitalWrite(pingPin4, LOW);        
  pinMode(pingPin4, INPUT);     
 
   duration4 = pulseIn(pingPin4, HIGH);
   
  inches = duration / 74 / 2;    // Convert to inches
  inches2 = duration2 / 74 / 2;  
  inches3 = duration3 / 74 / 2;  
  inches4 = duration4 / 74 / 2;
  Serial.println(inches);   

if(inches < 5){
  myservo.write(45);                    
}
delay(100);              // Short delay
if(inches2 < 5) {
  myservo.write(-45);
}
delay(100);
if(inches3 <5) {
  myservo2.write(45);
}
delay(100);
if(inches4 <5) {
  myservo2.write(-45);
}
delay(100);
}
