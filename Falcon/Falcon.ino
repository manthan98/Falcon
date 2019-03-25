#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include "Patient.h"
#include "Constants.h"

// Pin definitions
#define RST_PIN 9
#define SS_PIN 10

byte readCard[4];
Patient patients[2];
int patientsCount = 0;
String tagID = "";
String userInput;
boolean successRead = false;

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

    Serial.println(masterTagInputMessage);
    
    // Waits until a master card is scanned
    while (!successRead) {
        successRead = getID();
        if ( successRead == true) {
            char* masterID = strdup(tagID.c_str()); // Extract tag ID
            Patient masterPatient = { masterID, "1", "0" };
            patients[patientsCount] = masterPatient;
            patientsCount++;

            Serial.println(masterTagSetMessage);
            Serial.println(masterID);
        }
    }
    successRead = false;
    
    Serial.print(secondaryTagInputMessage);
    
    // Adds secondary user card
    while (!successRead) {
        successRead = getID();
        if ( successRead == true) {
            char* secondaryID = strdup(tagID.c_str()); // Extract tag ID
            Patient secondaryPatient = { secondaryID, "0", "0" };
            patients[patientsCount] = secondaryPatient;
            patientsCount++;

            Serial.println(secondaryTagSetMessage);
            Serial.println(secondaryID);
        }
    }
    successRead = false;
}

void loop() {

      checkForServerCommands();
  
      if ( !mfrc522.PICC_IsNewCardPresent() ) {
          // If a new PICC placed to RFID reader continue
          return;
      }
      
      if ( !mfrc522.PICC_ReadCardSerial() ) {
          //Since a PICC placed get Serial and continue
          return;
      }
      
      tagID = "";
      
      // The MIFARE PICCs that we use have 4 byte UID
      for ( uint8_t i = 0; i < 4; i++) {
        readCard[i] = mfrc522.uid.uidByte[i];
        tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
      }
      tagID.toUpperCase();
      mfrc522.PICC_HaltA(); // Stop reading

      // Check if scanned tag is allowed access
      if (tagID == patients[0].id && (patients[0].isAllowed == "1"))
      {
          Serial.print(accessGrantedMessage);
          operateDoor();
          patients[0].isInside = patients[0].isInside == "0" ? "1" : "0";
          Serial.println(patients[0].isInside);
      } else if (tagID == patients[1].id && (patients[1].isAllowed == "1"))
      {
          Serial.println(accessGrantedMessage);
          operateDoor();
          patients[1].isInside = patients[1].isInside == "0" ? "1" : "0";
          Serial.println(patients[1].isInside);
      } else
      {
          Serial.print(accessDeniedMessage);
      }
}

void checkForServerCommands() {
    if (Serial.available()) {
        userInput = Serial.readString();

        if (userInput == location) {
            // JSON format
            Serial.println("{ \"" + String(patients[0].id) + "\": \"" + patients[0].isInside + "\", \"" + String(patients[1].id) + "\": \"" + patients[1].isInside + "\" }");
        } else if (userInput == openDoor) {
            operateDoor();
        } else if (userInput == firstTagNoAccess || userInput == secondTagNoAccess) {
            for (uint8_t i = 0; i < patientsCount; i++) {
                if (String(patients[i].id) == userInput.substring(0, 8)) {
                    patients[i].isAllowed = "0";
                    Serial.println("Successfully updated tag ID: " + String(patients[i].id));
                }
            }
        } else if (userInput == firstTagWithAccess || userInput == secondTagWithAccess) {
            for (uint8_t i = 0; i < patientsCount; i++) {
                if (String(patients[i].id) == userInput.substring(0, 8)) {
                    patients[i].isAllowed = "1";
                    Serial.println("Successfully updated tag ID: " + String(patients[i].id));
                }
            }
        }
    }
}

void operateDoor() {
    // Unlock door
    servo.write(170);

    // Wait 8 seconds
    delay(8000);

    // Lock door
    servo.write(0);
}

uint8_t getID() {
    // Getting ready for Reading PICCs
    if ( !mfrc522.PICC_IsNewCardPresent() ) {
        // If a new PICC placed to RFID reader continue 
        return 0;
    }
    
    if ( !mfrc522.PICC_ReadCardSerial() ) {
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
