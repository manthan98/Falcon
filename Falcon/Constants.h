// Definitions for server commands sent to serial port
const String location = "LOCATION";
const String openDoor = "OPEN DOOR";
const String firstTagNoAccess = "733F511D: 0";
const String secondTagNoAccess = "CE6FC973: 0";
const String firstTagWithAccess = "733F511D: 1";
const String secondTagWithAccess = "CE6FC973: 1";

// Debugging commands for serial port;
const String masterTagInputMessage = "Scan and set the master tag";
const String secondaryTagInputMessage = "Scan and set the secondary access tag";
const String masterTagSetMessage = "Master tag set!";
const String secondaryTagSetMessage = "Secondary tag set!";
const String accessGrantedMessage = "Access granted";
const String accessDeniedMessage = "Access denied";
