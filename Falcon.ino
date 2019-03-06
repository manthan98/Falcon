#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 9
#define SS_PIN 10

byte readCard[4];
char* tags[2] = {};
int tagsCount = 0;
String tagID = "";
boolean successRead = false;
boolean correctTag = false;
boolean doorOpened = false;

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN); // RFID module
Servo servo; // Servo motor

void setup() {
    // Initiating
    Serial.begin(9600);
    SPI.begin();        // SPI bus
    mfrc522.PCD_Init(); //  MFRC522
    servo.attach(6);  // Servo motor
    servo.write(0); // Initial lock position of the servo motor

    Serial.print("Scan and set the master tag \n");
    
    // Waits until a master card is scanned
    while (!successRead) {
        successRead = getID();
        if ( successRead == true) {
          tags[tagsCount] = strdup(tagID.c_str()); // Sets the master tag into position 0 in the array
          Serial.print("Master Tag Set! \n");
          Serial.print(tags[tagsCount]);
          tagsCount++;
        }
    }
    successRead = false;

    Serial.print("Scan a secondary access tag \n");

    // Adds secondary user card
    while (!successRead) {
        successRead = getID();
        if ( successRead == true) {
          tags[tagsCount] = strdup(tagID.c_str()); // Sets the master tag into position 0 in the array
          Serial.print("Secondary Tag Set! \n");
          Serial.print(tags[tagsCount]);
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

      // Check if scanned tag is master tag
      if (tagID == tags[0])
      {
          Serial.print("Access granted \n");
        
          // Unlock door
          servo.write(135);

          // Wait 3 seconds
          delay(3000);

          // Lock door
          servo.write(0);
      } else
      {
          Serial.print("Access denied! \n");
      }
      
}

uint8_t getID() {
    // Getting ready for Reading PICCs
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        // If a new PICC placed to RFID reader continue 
        return 0;
    }
    
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        // Since a PICC placed get Serial and continue
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
    Serial.print("-Access Control- \n");
    Serial.print("Scan Your Tag! \n");
}
