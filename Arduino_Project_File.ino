// Martin Ofunrein
// May 2022
// Arduino Biometric Attendance System
// Collaborators: N/A
// Include necessary libraries for the fingerprint sensor, LCD display, and Ethernet
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Adafruit_Fingerprint.h>

// Setup for software serial communication
SoftwareSerial mySerial(2, 3);

// Initialize Adafruit fingerprint sensor library
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Network settings for Ethernet Shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC address
byte ip[] = {192, 168, 0, 100}; // Local IP address
byte serv[] = {192, 168, 0, 102} ; // Server IP address

// Initialize LCD screen (16 characters, 2 lines)
LiquidCrystal lcd(A1, A0, 8, 7, 6, 5);

// Create a client to connect to the server
EthernetClient cliente;

// Setup function runs once when you press reset or power the board
void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  
  // Start serial communication at 9600 bits per second
  Serial.begin(9600);
  
  // Start the Ethernet connection and the server
  Ethernet.begin(mac, ip);
  pinMode(A0, OUTPUT);

  // Begin communication with fingerprint sensor at 57600 bits per second
  finger.begin(57600);

  // Check if fingerprint sensor is available
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    lcd.println("Found sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    lcd.println("Not found :(");
    delay(10000);
  }
}

// This function reads a number from the serial input
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

// This function reads a string from the serial input
String readstring(void) {
  String str = "";

  while (str == "") {
    while (! Serial.available());
    str = Serial.readString();
  }
  return str;
}

// Main loop function that runs over and over again
void loop() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fingerprint");
  lcd.setCursor(0, 1);
  lcd.print("System");

  // If a finger is placed on the sensor, identify it
  if (digitalRead(A3) == HIGH) {
    int finger_id = getFingerprintID();

    Serial.print(finger_id);

    if (finger_id != 254) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome!" + finger_id);

      // If the connection is successful, send data to the server
      if (cliente.connect(serv, 80)) { 
        Serial.println("connected");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Connected");

        String query = "GET /fingerprint-database/data.php?";
        query += "id=";
        query += finger_id;
        query += "&in_out=";
        query += 1; 
        Serial.println(query);
        cliente.println(query);

        cliente.stop(); 
        Serial.print("Closed");
        delay(5000);
      }
      else {
        Serial.println("connection failed");
      }
    }
    delay(5000);
  }

  // If a finger is removed from the sensor, identify it
  if (digitalRead(A2) == HIGH) {
    int finger_id = getFingerprintID();

    Serial.print(finger_id);

    if (finger_id != 254) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome!" + finger_id);

      if (cliente.connect(serv, 80)) { 
        Serial.println("connected");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Connected");

        String query = "GET /fingerprint-database/data.php?";
        query += "id=";
        query += finger_id;
        query += "&in_out=";
        query += 0; 
        Serial.println(query);
        cliente.println(query);

        cliente.stop(); 
        Serial.print("Closed");
        delay(5000);
      }
      else {
        Serial.println("connection failed");
      }
    }
    delay(5000);
  }
  
}

// This function gets the fingerprint ID
uint8_t getFingerprintID() {
  // Follows Adafruit's fingerprint library sequence to identify a fingerprint

  // Start with getting the image from sensor
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    // Add other cases here for specific error messages
    // Complete this for all return codes from getImage()
  }
  
  // Convert the image to fingerprint template
  p = finger.image2Tz();
  // Check the result of image conversion and print appropriate message
  // Complete this for all return codes from image2Tz()

  // Search the template in the library
  p = finger.fingerFastSearch();
  // Check the result of search and print appropriate message
  // Complete this for all return codes from fingerFastSearch()
  
  // Print the ID if found
  if (p == FINGERPRINT_OK) {
    Serial.print("Found ID #"); Serial.print(finger.fingerID);
    Serial.print(" with confidence of "); Serial.println(finger.confidence);
  }
  
  return finger.fingerID;
}

// Simplified function to get fingerprint ID
int getFingerprintIDez() {
  // Follows same sequence as above function but without error checking
  // If anything fails, it immediately returns -1 (error)
  
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
