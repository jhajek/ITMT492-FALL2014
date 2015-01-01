/*
 *
 * FINAL PROJECT
 *
 */

/*
 **********************************************************************
 * LIBRARIES **********************************************************
 **********************************************************************
 */
//Libraries for RFID card reader
#include <Wire.h>
#include <Adafruit_NFCShield_I2C.h>
#include <LiquidCrystal.h> //LCD screen

//Libraries for real time clock (RTC)
#include <Wire.h>
#include "RTClib.h"
#include <string.h>
#include <SD.h>

//char *ReadCell(File Filee, int Row,int Column); 
/*
 **********************************************************************
 * SD Card Variables **********************************************************
 **********************************************************************
 */

  char delimiter = ',';
  int LineNumber = 1; //Row # Begins in 1 because Line number = 1 equals Row = 1 in excel. This is going to be equal to the total number of rows.
  int CommaNumber=0; //Column # Begins in 0 because, as soon as there is a comma, this is going to be comma 1 of the column
  int CommasPerLine=0;//Counts the number of commas per line. IT MEANS THE EXCEL FILE HAS TO BE SQUARE! It has to be equal to CommaNumber+1 because CommaNumber starts counting on 0
  int DelimiterCoord[15][15];
  int NewLineCoord[15];
  char FinalString[25]; //This is going to be its permanent size, so it has to hold a string of any length here.
  
  int IDRowStart=3;//3
  int IDRowEnd=9;
  int IDColumn=2;
  int ClassRowStart=0;
  int ClassRowEnd=0;
  int ClassColumn=0;

/*
 **********************************************************************
 * VARIABLES **********************************************************
 **********************************************************************
 */
//Variables for RFID card reader
#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield
LiquidCrystal lcd(4, 5, 6, 7, 8, 9); //LCD screen
Adafruit_NFCShield_I2C nfc(IRQ, RESET);
uint8_t success;
uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
// id to write in a card. We must modify it with the value we want to write!!
char idToWrite[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// id obtained after reading a RFID card
char idRead[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//Variables for real time clock (RTC)
RTC_DS1307 RTC;
int currentTime[7] = { 0,  //Year
                       0,  //Month
                       0,  //Day
                       0,  //Hour
                       0,  //Minute
                       0,  //Second
                       0 };//Day of week, sunday = 0 Saturday = 6

//Variables for speaker
const int speakerPin = 13;

 int StartTimeMinutes;
 int EndTimeMinutes;
 int CurrentTimeMinutes;
 
/*
 **********************************************************************
 * SETUP FUNCTIONS ****************************************************
 **********************************************************************
 */
//sets up the RFID card reader
void rfid_setup(void) {
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  
  if (!versiondata) { // Check if there's a RFID board plugged
  
    Serial.print("Didn't find PN53x board");
    while (1); //halt
		
  } else { // RFID board was found
	
    Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
    Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
    Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
		
    // configure board to read RFID tags
    nfc.SAMConfig();
    Serial.println("Waiting for an ISO14443A Card ...");
  }
}

//sets up the real time clock
void clock_setup() {
  Wire.begin();
  RTC.begin();
  
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
}
void clock_getCurrentTime();
/*
 **********************************************************************
 * SETUP **************************************************************
 **********************************************************************
 */
void setup() {
  Serial.begin(57600);
  lcd.begin(16, 2);  //set up LCD screen
  rfid_setup();      //set up the RFID card reader 
  clock_setup();     //set up the real time clock
  pinMode(speakerPin, OUTPUT); //set up the speaker
  if (!SD.begin(53)) {
    Serial.println("SD Card Initialization failed! (Setup Failed :/ )");
    return;
  }
  Serial.println("SD Reader Initialization done. (Setup Worked!)");
}

/*
 **********************************************************************
 * REST OF THE FUNCTIONS **********************************************
 **********************************************************************
 */
//
//Reads data from a card when it is found
//Returns true if the reading was done correctly
//
boolean rfid_readFromCard() {

  boolean readingSuccesful = false;
	
  Serial.println("Trying to authenticate block 4 with default KEYA value");
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
	
  if (success) {
	
    Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
    uint8_t data[16];

    // Try to read the content of block 4
    success = nfc.mifareclassic_ReadDataBlock(4, data);
    
    if (success) {
      readingSuccesful = true;
      // Data seems to have been read ... show it on screen
      Serial.println("Reading Block 4:");
      nfc.PrintHexChar(data, 16);
      Serial.println("");
      
      for(int i = 0; i < 16; i++) {
        idRead[i] = data[i];
      }
      Serial.print("Data in string format: ");
      for(int i = 0; i < 16; i++) {
        Serial.print(idRead[i]);
      }
      Serial.println("");
      Serial.println("");
			
      } else {
        Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        asm volatile ("  jmp 0");
      }
		
    } else {
      Serial.println("Ooops ... authentication failed: Try another key?");
      asm volatile ("  jmp 0");
    }

  return readingSuccesful;
}
 
//gets current date and puts it inside the currentTime[] array
void clock_getCurrentTime() {
  DateTime now = RTC.now();
  
  currentTime[0] = now.year();
  currentTime[1] = now.month();
  currentTime[2] = now.day();
  currentTime[3] = now.hour();
  currentTime[4] = now.minute();
  currentTime[5] = now.second();
  currentTime[6] = now.dayOfWeek();
  
  Serial.print(currentTime[1], DEC);
  Serial.print('/');
  Serial.print(currentTime[2], DEC);
  Serial.print('/');
  Serial.print(currentTime[0], DEC);
  Serial.print(' ');
  Serial.print(currentTime[3], DEC);
  Serial.print(':');
  Serial.print(currentTime[4], DEC);
  Serial.print(':');
  Serial.print(currentTime[5], DEC);
  Serial.print("    Day of the week: ");
  Serial.print(currentTime[6], DEC);
  Serial.println();
}

//plays a sound meaning that a process was successful
void speaker_YESbeep() {
  for(int i = 0; i < 500; i++) { //sets the duration of the beep
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(400); //sets the frequency of the beep
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(400);
  }
  for(int i = 0; i < 500; i++) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(500);
  }
}

//plays a sound meaning that a process was NOT successful
void speaker_NObeep() {
  for(int i = 0; i < 150; i++) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(500);
  }
  delay(25);
  for(int i = 0; i < 150; i++) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(500);
  }
}


/*
 **********************************************************************
 * LOOP ***************************************************************
 **********************************************************************
 */
void loop() {
	clock_getCurrentTime();
    /*
     * PRINT CURRENT DATE ON LCD SCREEN (STANDBY MESSAGE)
     */
    lcd.print("***SWIPE CARD***"); //Display student ID on LCD screen
    lcd.setCursor(0,1);
    if(currentTime[3] >= 10){lcd.print(currentTime[3]);}else{lcd.print("0");lcd.print(currentTime[3]);}
    lcd.print(":");
    if(currentTime[4] >= 10){lcd.print(currentTime[4]);}else{lcd.print("0");lcd.print(currentTime[4]);}
    lcd.print(" ");
    if(currentTime[1] >= 10){lcd.print(currentTime[1]);}else{lcd.print("0");lcd.print(currentTime[1]);}
    lcd.print("/");
    if(currentTime[2] >= 10){lcd.print(currentTime[2]);}else{lcd.print("0");lcd.print(currentTime[2]);}
    lcd.print("/");
    lcd.print(currentTime[0]);
    lcd.setCursor(0,0);

  //look for RFID card
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  File fi = SD.open("students.csv",FILE_WRITE);
  if(success) { //RFID card found!
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
		
    if(uidLength == 4) { //The RFID card is a Mifare Classic card		
      
      rfid_readFromCard(); //read student ID and pass it to idRead[]
      
      if(idRead[0] != 0) { //reading was successful!
        if(idRead[0] == '0') {
          speaker_YESbeep(); 
        } else {
          lcd.clear();lcd.print("****WRONG ID****");lcd.setCursor(0,1);lcd.print("****************");lcd.setCursor(0,0); 
          speaker_NObeep(); 
        }

        clock_getCurrentTime(); //now currentTime[] has time data on it
        //SD CODE BEGIN//
          //File fi = SD.open("students.csv",FILE_WRITE);//FILE_WRITE means opening for both reading and writing. It also starts at the end, so we have to fix that.
              if (fi) {
                SDCard(fi);
              } else {
                Serial.println("error opening students csv!!");//Print error if file didn't open.
                delay(500);
                asm volatile ("  jmp 0");
              } //Closes initial IF
        //SD CODE ENDS//
        //reset idRead[]
        for(int i = 0; i < 16; i++) {
          idRead[i] = 0;
        }
        //reset currentTime[]
        for(int i = 0; i < 6; i++) {
          currentTime[i] = 0;
        }
        // Wait a bit before reading the card again
        delay(2000);
        
      }
    } 
		
    if(uidLength == 7) { //The RFID card is a MiFare Ultralight Card (WE AREN'T USING THEM)
      Serial.println("We aren't working with MiFare Ultralight Cards!!");
    }
  } else {
    //no RFID card found. Do nothing.
  }
  /*
   * NEW CODE FOR CHECKING ABSENCE
   */
    if(fi) {
    readEndTime(fi);
    clock_getCurrentTime();
    if(CurrentTimeMinutes > EndTimeMinutes + 15){
      //if it's more than 15 minutes after the end of the class, check the table for absent
      Serial.println("It's been 15 minutes after class. Time to check for absence!");
      ckeck_absence(fi);
    } else {
      //do nothing, the class has to end and we'll check absence later
    }

  }	
  
}

int SDCard(File Filee){
  Serial.println("Opened file with success. (SDCard Function Starting)");     
      Filee.seek(0);//This is because the cursor starts in the end.
    ProcessFile(Filee);
    CompareIDColumn(Filee);

    Filee.close();
    delay(5000);//Delay between each sd card loop.
      asm volatile ("  jmp 0");//Resetting, because arduino is crazy
}

int ProcessFile(File Filee){//This function sets up all necessary variables and creates the virtual model of the excel.
  Serial.print("Here is the full file:----------------------------\n");
  while (Filee.available()){
        char c = Filee.read();
        if (c == '\n'){//Here we increment the LineNumber(ROW) exactly when we find a newline. The downside is that the last newline is useless. But this is solved by a -1 later. We also reset CommaNumber to 0, to start counting on the next line.
      NewLineCoord[LineNumber]=Filee.position();//Saves the position JUST AFTER newlines.
        LineNumber++;
        CommasPerLine=CommaNumber;
        CommaNumber=0;
        } else if (c == delimiter){ // You can just directly compare CHARS in C. Here, we make sure the current character is equal to our delimiter (,), if it is, we increase the delimiter number and record its location.
        CommaNumber++;
        DelimiterCoord[LineNumber][CommaNumber]=Filee.position(); 
        } 
        Serial.print(c);//Just for debugging purposes.
  }
  Serial.print("End of file:--------------------------------------\n");
  LineNumber=LineNumber-1; //This is because the last line is always empty, so we shouldn't really count it.
  //Serial.print("\nLast and total LineNumber:");Serial.print(LineNumber);
  //Serial.print("\nLast CommaNumber:");Serial.print(CommaNumber);
  //Serial.print("\nCommasPerLine:");Serial.print(CommasPerLine);Serial.println("\n");
}

char *ReadCell(File Filee,int Row,int Column){//Procedure: Call ReadCell with the file, row and column, and it will return nothing. Instead, it will change a global variable (FinalString) that has a SET size.
  int StartPosition;
  int EndPosition;
  int TotalRows=CommasPerLine+1; //The rows are all the commas plus one. Since the last space does not have a comma after it, just a CRLF
  char buf[123]="";
  int StartingArraySize=0;
  if (Column != 1 && Column != TotalRows){Column--;StartPosition=DelimiterCoord[Row][Column];EndPosition=DelimiterCoord[Row][Column+1];} //Since we are passing it to a function that uses the number of comma, we have to decrease it to get the ACTUAL space number. So, Comma 2 - Comma 1 = Space 2.
  else if (Column == TotalRows){Column--;StartPosition=DelimiterCoord[Row][Column];EndPosition = NewLineCoord[Row];}//If we are in the LAST column there are no limiting commas there, so instead we use CRLF position - Last comma = Last Space = First space
  else if (Column == 1){ //If we are in the first comma, there are also no limiting commas before it. So we take the FIRST comma - LAST CRLF = First Space. If its in the first line though, We just do FIRST comma - 0(first position)
    if (Row==1){StartPosition=0;}
    else{StartPosition=NewLineCoord[Row-1];}
    EndPosition=DelimiterCoord[Row][Column];
  }
  Filee.seek(StartPosition);
  if (Column == CommasPerLine){ //If the column is the LAST column
    if (Row == LineNumber){ //If the required row is equal to the TOTAL number of rows (last row)
      //Serial.println("Error. This is the end of the file.");
      //return 0;
    }
    Row++;
    Column=0;
    }//If its in the end of any line, it deals with that by going to the next line
  int FinalStringSize = EndPosition-StartPosition;
  int currentIndex=0;
  while(1){
    char c = Filee.read();
    if (Filee.position()==EndPosition){
    break;
    }
    //Serial.print(c); //Printing char by char  
    FinalString[currentIndex]=c; //Creating the array element by element
    //Serial.println(currentIndex);
    currentIndex++;
    //Serial.println("Ok, first checkpoint...");
  }
  FinalString[FinalStringSize-1]='\0';//Last char of an array is always this one.
  //Serial.print("FinalString:");Serial.print(FinalString);Serial.print("\n");
}

int CompareStrings(int NumberOfCharacters){//Number of characters to compare. (MON = 3, 00000009 = 8)EXPECTS FinalString and idRead to exist. Compares two strings to see if they are equal. The SECOND array's size is compared to the FIRST array size. This is how it differs from the normal array comp function.
  int buffer=0;
  Serial.print("");//This makes the code work. I have no idea why, but lets leave it there.
  //Serial.print("Starting CompareStrings:\n");Serial.print("sizeof(FinalString):");Serial.print(sizeof(FinalString));Serial.print("\n");Serial.print("sizeof(idRead):");Serial.print(sizeof(idRead));Serial.print("\n");//Prints the size of the involved parties
  for (int i =0;i< NumberOfCharacters;i++){
    //Serial.print("FinalString[i]:");Serial.print(FinalString[i]);Serial.print("\n");Serial.print("     idRead[i]:");Serial.print(idRead[i]);Serial.print("\n");
            if (FinalString[i]!=idRead[i]){return 0;}else{buffer++;}
            if (buffer == (sizeof(FinalString))){return 1;}
          }
}

int CompareIDColumn(File Filee){//Compares the full column of student IDS
  for (int i=IDRowStart;i<=IDRowEnd;i++){
    ReadCell(Filee,i,IDColumn);
    if (CompareStrings(8)){
      //Debugging string: Shows how many matches, who are the matches and the row of the matches.
       readStartTime(Filee);
       readEndTime(Filee);
       int dayWeek = getDayOfWeek(Filee); //read the day of the week       
       //Serial.print("day of the week of the class = ");Serial.println(dayWeek);Serial.print("actual day of the week = ");Serial.println(currentTime[6]);
       if(dayWeek == currentTime[6] && isClassTime()) {
         if(studentIsLate()){
           ReadCell(Filee,i,1);Serial.print(FinalString);Serial.println(" is late! (I wrote 2 on the file)\n");
           lcd.clear();lcd.print(FinalString); lcd.setCursor(0,1); lcd.print("Arrived late"); lcd.setCursor(0,0);
           Filee.seek(DelimiterCoord[i][IDColumn+2]);
           Filee.write("2");
         } else {
           ReadCell(Filee,i,1);Serial.print(FinalString);Serial.println(" arrived on time! (I wrote 1 on the file)\n");
           lcd.clear();lcd.print(FinalString); lcd.setCursor(0,1); lcd.print("Arrived on time"); lcd.setCursor(0,0);
           Filee.seek(DelimiterCoord[i][IDColumn+2]);//Preparing to write. Change this to reflect what day it is. Starting on ID column = starting on "day 1"
           Filee.write("1");
           delay(2000);
         }
       } else if(dayWeek == currentTime[6] && (CurrentTimeMinutes >= EndTimeMinutes-15)){ // If the guy arrives 15 mins before the class ends until the end of the day he is absent.
         ReadCell(Filee,i,1);Serial.print(FinalString);Serial.println(" received an absent mark :(! (I wrote 0 on the file)\n");
         lcd.clear();lcd.print(FinalString); lcd.setCursor(0,1); lcd.print("Was absent"); lcd.setCursor(0,0);
         Filee.seek(DelimiterCoord[i][IDColumn+2]);
         Filee.write("0");
         delay(2000);
       } else if (dayWeek != currentTime[6]){ //Day is wrong.
         Serial.println("Class isn't held this day of the week!");
         lcd.clear();lcd.print("CLASS ISN'T HELD"); lcd.setCursor(0,1); lcd.print("THIS DAY OF WEEK");
         delay(2000);
       } else { //Before class starts
         Serial.println("Class hasn't already started!");
         lcd.clear();lcd.print("**CLASS HASN'T**"); lcd.setCursor(0,1); lcd.print("ALREADY  STARTED");
         delay(2000);
       }
    }
  }
}

/*
 * NEW METHODS FOR TIME COMPARISON
 */

void readStartTime(File Filee) {
  int startTimeColumn = 2;int startTimeRow = 2;
  ReadCell(Filee, startTimeRow, startTimeColumn); // now FinalString[] = {'1','5',':','3','0',0,0,0,...,0} if the start time is 15:30
  StartTimeMinutes=((FinalString[0]-'0')*10+(FinalString[1]-'0'))*60+(FinalString[3]-'0')*10+(FinalString[4]-'0'); // Gets the time in minutes, so 10h 30m = 630 m
}

void readEndTime(File Filee){
  int endTimeColumn = 3;int endTimeRow = 2;
  ReadCell(Filee, endTimeRow, endTimeColumn); // now FinalString[] = {'1','6',':','4','5',0,0,0,...,0} if the start time is 16:45
  EndTimeMinutes=((FinalString[0]-'0')*10+(FinalString[1]-'0'))*60+(FinalString[3]-'0')*10+(FinalString[4]-'0'); // Gets the time in minutes, so 10h 30m = 630 m
}


boolean isClassTime() {
	CurrentTimeMinutes=(currentTime[3]*60+currentTime[4]);
	
	/*
	Serial.print("EndTimeMinutes = ");Serial.println(EndTimeMinutes);
	Serial.print("CurrentTimeMinutes = ");Serial.println(CurrentTimeMinutes);
	Serial.print("StartTimeMinutes = ");Serial.println(StartTimeMinutes);
	*/

	if (CurrentTimeMinutes >= StartTimeMinutes && CurrentTimeMinutes <= EndTimeMinutes){
	return true;
	} else {
		return false;
	}

}

boolean studentIsLate() {
	int timeLimitMinutes;
	timeLimitMinutes=StartTimeMinutes+15;
	if (CurrentTimeMinutes>=timeLimitMinutes){
		return true;
	} else{
		return false;
	}
}


int getDayOfWeek(File Filee) {
  int dayColumn = 4;
  int dayRow = 2;
  ReadCell(Filee, dayRow, dayColumn); //now FinalString[] = {'M','O','N',0,0,0,0,0,...,0} if the day is MON (Monday)
  idRead[0]='M';idRead[1]='O';idRead[2]='N';
  if(CompareStrings(3)){return 1;}
  idRead[0]='T';idRead[1]='U';idRead[2]='E';
  if (CompareStrings(3)){return 2;}
  idRead[0]='W';idRead[1]='E';idRead[2]='D';
  if (CompareStrings(3)) {return 3;}
  idRead[0]='T';idRead[1]='H';idRead[2]='U';
  if (CompareStrings(3)) {return 4;}
  idRead[0]='F';idRead[1]='R';idRead[2]='I';
  if (CompareStrings(3)) {return 5;}
  idRead[0]='S';idRead[1]='A';idRead[2]='T';
  if (CompareStrings(3)) {return 6;}
  idRead[0]='S';idRead[1]='U';idRead[2]='N';
  if (CompareStrings(3)) {return 0;} 
  else {
    Serial.println("I can't recognize the day, something is wrong.");
    return -1;
  }
}

/*
* NEW METHOD FOR CHECKING ABSENCE
*/
void ckeck_absence(File Filee) { //reads the table until it finds an 'N' character. Then checks the first column with 'N's for absences
    int firstNcolumn = 0; // column to check for absences
    
    for(int column = 0; column < CommasPerLine; column++) {
      for(int row = 0; row < LineNumber; row++) {
        ReadCell(Filee, row, column); // now FinalString[0] = '0', '1', '2' or 'N'
        if(FinalString[0] == 'N') {
          firstNcolumn = column; //we found the column with the first 'N'!
          break;
        }
      }
    }
    
    if(firstNcolumn != 0) {
      Serial.print("firstNcolumn = ");Serial.println(firstNcolumn);
      boolean allN = true;
      for(int row = 4; row < LineNumber; row++) {
        ReadCell(Filee, row, firstNcolumn);
        if(FinalString[0] != 'N') {
          allN = false;
        }
      }
      if(allN) {
        //All columns in that row are 'N'
        //this means this is a whole new class day
        //no need to check for absence!
      } else {
        Serial.println("There was some student absent! let's write a 0 on his cell");
        for(int row = 4; row < LineNumber; row++) {
          ReadCell(Filee, row, firstNcolumn);
          if(FinalString[0] == 'N') {
            //the student was absent!!
            //write a '0' on this place in the file
            Filee.seek(DelimiterCoord[row][firstNcolumn]);
            Filee.write("0");
          }
        }
      }
    }
    
}//end check_absence(File Filee)
