#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>


#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>


const char* ssid = "Xiaomi";
const char* password = "Hitha@13";

// Use UART2 on ESP32
HardwareSerial mySerial(2); // UART2
Adafruit_Fingerprint finger(&mySerial);
bool hasVoted[128] = {false}; // supports up to 128 fingerprint IDs


void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for Serial Monitor to connect

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to Wi-Fi!");


  mySerial.begin(57600, SERIAL_8N1, 16, 17);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("✅ Fingerprint sensor found!");
  } else {
    Serial.println("❌ Fingerprint sensor not found!");
    while (1) delay(1);
  }
}

void enrollFingerprint(uint8_t id) {
  int p = -1;
  Serial.print("Checking if fingerprint is already enrolled...\n");

  // Ask user to place finger first
  Serial.println("Place your finger to check for duplicates...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert image to template.");
    return;
  }

  // Search existing templates
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("🚫 This fingerprint is already enrolled with ID: ");
    Serial.println(finger.fingerID);
    return;
  } else {
    Serial.println("✅ Fingerprint is not enrolled, proceeding with enrollment...");
  }

  // Redo the process for enrollment
  Serial.println("Remove finger...");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    delay(100);
  }

  // Begin actual enrollment steps
  Serial.println("Place finger for first scan...");
  while (finger.getImage() != FINGERPRINT_OK);
  finger.image2Tz(1);

  Serial.println("Remove finger...");
  delay(2000);
  while (finger.getImage() != FINGERPRINT_NOFINGER) {
    delay(100);
  }

  Serial.println("Place the same finger again...");
  while (finger.getImage() != FINGERPRINT_OK);
  finger.image2Tz(2);

  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to create model.");
    return;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("🎉 Fingerprint enrolled successfully!");
  } else {
    Serial.println("❌ Failed to store fingerprint.");
  }
}

void sendVoteToServer(int voterId) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.121.56:3000/vote");  // Replace <YOUR_PC_IP>
    http.addHeader("Content-Type", "application/json");
    String payload = "{\"voterId\":" + String(voterId) + "}";
    int httpResponseCode = http.POST(payload);
    Serial.println(" Sent vote to server. Response code: " + String(httpResponseCode));
    http.end();
  } else {
    Serial.println(" Wi-Fi not connected. Cannot send vote.");
  }
}

void verifyFingerprint() {
  int p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println("No finger detected or imaging error.");
    return;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert image.");
    return;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    uint8_t id = finger.fingerID;
    Serial.print("✅ Verified! User ID: ");
    Serial.println(id);

    if (hasVoted[id]) {
      Serial.println("⚠️ This user has already voted!");
    } else {
      Serial.println("🗳️ Casting vote...");
      hasVoted[id] = true;
      
      //Call blockchain function here
      sendVoteToServer(id);

      Serial.println("✅ Vote cast successfully!");
    }
  } else {
    Serial.println("❌ Fingerprint not recognized.");
  }
}


void deleteFingerprint(uint8_t id) {
  int p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print("🗑️ Successfully deleted fingerprint with ID: ");
    Serial.println(id);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("❌ Communication error while deleting.");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("❌ Invalid ID. No such fingerprint stored.");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("❌ Flash error while deleting.");
  } else {
    Serial.println("❌ Unknown error.");
  }
}


void loop() {
  Serial.println("\n--- Menu ---");
  Serial.println("1: Enroll a new fingerprint");
  Serial.println("2: Verify fingerprint");
  Serial.println("3: Delete a fingerprint");
  Serial.println("4: Exit");

  while (Serial.available() == 0);
  char choice = Serial.read();

  if (choice == '1') {
    Serial.print("Enter user ID to enroll: ");
    while (Serial.available() == 0);
    int id = Serial.parseInt();
    enrollFingerprint(id);
  } 
  else if (choice == '2') {
    verifyFingerprint();
  } 
  else if (choice == '3') {
    Serial.print("Enter ID to delete: ");
    while (Serial.available() == 0);
    int id = Serial.parseInt();
    deleteFingerprint(id);
  }
  else if (choice == '4') {
    Serial.println("Exiting...");
    while (1);  // halt
  } 
  else {
    Serial.println("Invalid option.");
  }

  delay(2000);
}
