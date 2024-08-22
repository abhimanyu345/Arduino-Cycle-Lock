#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

Servo lockServo;  // Create a servo object

// Define your authorized RFID tag UID here
byte authorizedUID[] = {0xDE, 0xAD, 0xBE, 0xEF}; // Change this to your RFID tag UID

void setup() {
  Serial.begin(9600); // Initialize serial communication
  SPI.begin();        // Initialize SPI bus
  mfrc522.PCD_Init(); // Initialize MFRC522
  lockServo.attach(6); // Attach the servo to pin 6
  lockServo.write(0); // Start with lock in closed position
  Serial.println("Scan your RFID tag");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  bool authorized = true;
  for (byte i = 0; i < sizeof(authorizedUID); i++) {
    if (mfrc522.uid.uidByte[i] != authorizedUID[i]) {
      authorized = false;
      break;
    }
  }

  if (authorized) {
    Serial.println("Access Granted!");
    lockServo.write(90); // Unlock (open the lock)
    delay(5000); // Keep unlocked for 5 seconds
    lockServo.write(0); // Lock (close the lock)
  } else {
    Serial.println("Access Denied!");
  }
  
  mfrc522.PICC_HaltA();  // Halt PICC
  mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
}
