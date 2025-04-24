#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

#define LED_GREEN 3
#define LED_RED 2
#define BUZZER_PIN A5

MFRC522 rfid(SS_PIN, RST_PIN);

// TODO: Check UID size (rfud.uid.size) and set UID_SIZE accordingly
#define UID_SIZE 4
byte uidTag1[UID_SIZE];
byte uidTag2[UID_SIZE];

bool isUidTag1Saved = false;
bool isUidTag2Saved = false;

byte uidTag[UID_SIZE];

bool compareUids(byte* uid1, byte* uid2) {
  for (int i = 0; i < UID_SIZE; i++) {
    if (uid1[i] != uid2[i]) {
        return false;
    }
  }
  return true;
}

/// @brief Reads the RFID tag and returns true if a new card is present and read successfully.
/// @param tag Tag UID will be stored here.
/// @return Returns true if a new card is present and read successfully, false otherwise.
bool rfidTagRead(byte tag[]) {
  bool isRead = rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial();
  if (isRead) {
    for (int i = 0; i < UID_SIZE; i++) {
      tag[i] = rfid.uid.uidByte[i];
    }
  }
  return isRead;
}

void printRfidUid(byte tag[]) {
  for (byte i = 0; i < UID_SIZE; i++) {
    Serial.print(tag[i] < 0x10 ? " 0" : " ");
    Serial.print(tag[i], HEX);
  }
  Serial.println();
}

/// @brief Saves the UID of the RFID tag to the destination array if it is not already saved.
/// @param dest Destination array where the UID will be saved.
/// @param src Source array containing the UID to be saved.
/// @param isTagSaved  Reference to a boolean variable indicating if the tag is already saved.
/// @note src and dest should be of size `UID_SIZE`.
void pairTag(byte dest[], byte src[], bool &isTagSaved){
  if (isTagSaved) {
    Serial.println("Tag already saved.");
    return;
  }
  for (int i = 0; i < UID_SIZE; i++) {
    dest[i] = src[i];
  } 
  isTagSaved = true;
}

void pairTags(byte tag[]) {
  if (isUidTag1Saved && isUidTag2Saved) {
    Serial.println("Both tags have already been saved.");
    return;
  }

  if (!isUidTag1Saved) {
    pairTag(uidTag1, tag, isUidTag1Saved);
    Serial.println("Tag 1 has been saved.");
    Serial.println("Now place the second tag to save it as Tag 2.");

  } else if (!compareUids(rfid.uid.uidByte, uidTag1)) {
    Serial.println("This tag is already saved as Tag 1. Place another, unique tag.");

  } else if (!isUidTag2Saved){
    pairTag(uidTag2, tag, isUidTag2Saved);
    Serial.println("Tag 2 has been saved.");
    Serial.println("Both tags have been saved. Now the program will react to them accordingly.");
  }
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, HIGH);


  Serial.println("Place the first RFID tag to save it as Tag 1...");
}

void handleTag(int tagID, int ledPin, int buzzerFreq) {
  Serial.print("Detected Tag: ");
  Serial.println(tagID);
  digitalWrite(ledPin, LOW);
  tone(BUZZER_PIN, buzzerFreq, 1000);
  delay(1000);
  digitalWrite(ledPin, HIGH);
  noTone(BUZZER_PIN);
}

void handleTags(byte tag[]) {
  if (compareUids(tag, uidTag1)) {
    handleTag(1, LED_GREEN, 900);
  }
  else if (compareUids(tag, uidTag2)) {
    handleTag(2, LED_RED, 200);
  } 
  else {
    Serial.println("Unknown tag.");
  }
}

void loop() {
  if(!rfidTagRead(uidTag)) {
    return;
  }

  Serial.print("Read tag with UID: ");
  printRfidUid(uidTag);
  Serial.println();

  if(!isUidTag1Saved || !isUidTag2Saved) {
    pairTags(uidTag);
  } else {
    handleTags(uidTag);
  }

  rfid.PICC_HaltA();
  // rfid.PCD_StopCrypto1();
}