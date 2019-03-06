#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 9
#define SS_PIN 10

byte readCard[4];
char* myTags[100] = {};
int tagsCount = 0;
String tagID = "";
boolean successRead = false;
boolean correctTag = false;
boolean doorOpened = false;

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo; // Servo motor

void setup() {
    Serial.begin(9600);
  
    // Initiating
    SPI.begin();        // SPI bus
    mfrc522.PCD_Init(); //  MFRC522
    myServo.attach(6);  // Servo motor
    myServo.write(0); // Initial lock position of the servo motor
    
    // Waits until a master card is scanned
    while (!successRead) {
        successRead = getID();
        if ( successRead == true) {
          myTags[tagsCount] = strdup(tagID.c_str()); // Sets the master tag into position 0 in the array
          Serial.print("Master Tag Set!");
          Serial.print(myTags[tagsCount]);
          tagsCount++;
        }
    }
    successRead = false;
    printNormalModeMessage();
}

void loop() {
  
      if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
        return;
      }
      
      if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
        return;
      }
      
      tagID = "";
      // The MIFARE PICCs that we use have 4 byte UID
      for ( uint8_t i = 0; i < 4; i++) {  //
        readCard[i] = mfrc522.uid.uidByte[i];
        tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
      }
      tagID.toUpperCase();
      mfrc522.PICC_HaltA(); // Stop reading

      if (tagID == myTags[0])
      {
          Serial.print("NICE!");
          myServo.write(45);
          delay(1000);
          myServo.write(0);
      }
      
}

uint8_t getID() {
    // Getting ready for Reading PICCs
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        // If a new PICC placed to RFID reader continue 
        return 0;
    }
    
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        //Since a PICC placed get Serial and continue
        return 0;
    }
    
    tagID = "";
    for ( uint8_t i = 0; i < 4; i++) {
        // The MIFARE PICCs that we use have 4 byte UID
        readCard[i] = mfrc522.uid.uidByte[i];

        // Adds the 4 bytes in a single String variable
        tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    
    tagID.toUpperCase();
    mfrc522.PICC_HaltA(); // Stop reading
    
    return 1;
}

void printNormalModeMessage() {
    delay(1500);
    Serial.print("-Access Control-");
    Serial.print("Scan Your Tag!");
}
