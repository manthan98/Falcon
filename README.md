# Falcon

Falcon is a patient monitoring system for dementia patients in long-term care facilities. The hardware is responsible for detecting
authorized patient RFID tags, and driving the door-lock hardware (a simple servo motor) to either open or close.

## Circuit

The hardware makes use of an Arduino Uno, MFRC522 RFID module and TowerPro SG92R servo motor. The following circuit setup is required for the firmware:

![Screen Shot 2019-04-17 at 5 01 07 PM](https://user-images.githubusercontent.com/19896167/56320592-86d6e480-6132-11e9-8815-fa003c42a8db.png)

## Usage

Construct the circuit setup defined, and run the code in the Arduino application. Please ensure that Arduino [RFID library](https://github.com/miguelbalboa/rfid) for the MFRC522 module is installed.

## Architecture

The firmware can be separated into three components: detection, operation, and server communication. The communication between the RFID module and Arduino is via the
SPI protocol. At different phases of the components, several messages are logged to the serial monitor.

The detection process is initiated by setting a master tag, which is authorized for access, and a secondary tag which is 
not authorized. This process is handled in the Arduino setup loop. Each tag is modelled as a `Patient` object with three key parameters: 

* `id`: 4 byte representation of a RFID tag through a character array.
* `isAllowed`: if patient is allowed inside room.
* `isInside`: if the patient is inside or outside the room 

Upon setup, each tag is mapped to a new patient object.

The operation process involves decoding the RFID tags and their authorization, as well as facilitating communication with a companion web
application found [here](https://github.com/elmico377/Falcon-Web). Every time a RFID tag is tapped on the module, its 4 byte encoding is extracted and compared with the
set of tags in-memory (namely master and secondary tag IDs). During the comparison, if the detected ID matches one of the in-memory tag IDs,
and the corresponding tag ID has authorization, the door lock hardware unlocks; otherwise it stays locked.

Additionally, the firmware also facilitates instruction handling from a web server via the serial monitor. The `checkForServerCommands()`
function reads from the serial monitor and handles particulate incoming messages:

* `LOCATION`: sends the current location of all patients in-memory (`isInside` parameter).
* `OPEN DOOR`: an override command that forces the door-lock hardware open regardless of tag authorization.

In addition to these, the web server can control a specific patient's authorization (via the tag ID), and correspondingly update the authorization
of a tag in-memory on the Arduino.
