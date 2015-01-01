import processing.serial.*;
//import java.sql.*;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.logging.Level;
import java.util.logging.Logger;

Serial myPort;   // The serial port

int yDist;
PFont font12;
PFont font24;
float[] tempHistory = new float[100];

int addr1;
int addr2;
int addr3;
int addr4;

void setup() {
 
  //setup fonts for use throughout the application
  font12 = loadFont("Verdana-12.vlw");
  font24 = loadFont("Verdana-24.vlw");
 
  //set the size of the window
  size(550, 250);
 
 
  //fill tempHistory with default temps
  for(int index = 0; index<100; index++)
  tempHistory[index] = 0;

  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  print(Serial.list());
  String portName = Serial.list()[2];
  myPort = new Serial(this, portName, 9600);
}

void draw() {
   
   if (myPort.available() >= 16) { // Wait for coordinator to recieve full XBee frame
   if (myPort.read() == 0x7E) { // Look for 7E because it is the start byte
    for (int i = 1; i<15; i++) { // Skip through the frame to get to the unique 32 bit address
      //get each byte of the XBee address
      if(i == 8) { addr1 = myPort.read(); }
      else if (i==9) { addr2 = myPort.read(); }
      else if (i==10) { addr3 = myPort.read(); }
      else if (i==11) { addr4 = myPort.read(); }
      else { int discardByte = myPort.read(); } //else throwout byte we don't need it
    }
    int sensorData = myPort.read(); // Read the first analog byte data
    //int analogLSB = myPort.read(); // Read the second byte
    //float volt = calculateXBeeVolt(analogMSB, analogLSB);//Convert analog values to voltage values
    println(indentifySensor(addr1,addr2,addr3,addr4)); //get identity of XBee and print it
    print("Distance in cm: ");
    println(sensorData);
  }
}
}

//Function takes in the XBee address and returns the identity of the Xbee that sent the temperature data
String indentifySensor(int a1, int a2, int a3, int a4) {
  int rout1[] = {64, 177, 132, 227}; //Arrays are the 32 bit address of the two XBees routers
  int end1[] = {64, 167, 69, 143};
  int end2[] = {64, 166, 42, 94};
  if(a1==rout1[0] && a2==rout1[1] && a3==rout1[2] && a4==rout1[3]) { //Check if Sensor 1
  return "Distance from router - sensor 1"; } //temp data is from XBee one
  else if(a1==end1[0] && a2==end1[1] && a3==end1[2] && a4==end1[3]) {//Check if Sensor 2
  return "Distance from router - sensor 2"; } //temp data is from XBee two
  else if(a1==end2[0] && a2==end2[1] && a3==end2[2] && a4==end2[3]) {//Check if Sensor 3
  return "Distance from route - sensor 3"; } //temp data is from XBee two
  else { return "I don't know this sensor"; }  //Data is from an unknown XBee
}

