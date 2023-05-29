#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

byte ledRed = 8;
byte ledGreen = 7;
byte buzzer = 4;
byte relay = 3;

MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

void setup() {
  //  serial monitor
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  Serial.println("Tap RFID/NFC Tag on reader");

  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(relay, OUTPUT);

  lcd.init(); // initialize the lcd
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("TEMPEL KARTU  ");
  lcd.setCursor(0, 1);
  lcd.print(" PINTU TERKUNCI  ");

  digitalWrite(relay, HIGH);
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been readed
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID/NFC Tag Type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));

      // print NUID in Serial Monitor in the hex format
      String uid = "UID: ";
      for (int i = 0; i < rfid.uid.size; i++) {
        uid += (rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1) uid += " ";
      }
      uid.toUpperCase();
      Serial.println(uid);

      if (uid == "UID: B3 B5 4C A9") {
        valid();
      }
      else if (uid == "UID: 04 1C 4C 9A 95 4C 80") {
        valid();
      }
      else if (uid == "UID: 03 02 C1 A6") {
        valid();
      }
      else {
        invalid();
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }
}

void valid () {
  Serial.println("Access Granted");
  Serial.println();
  delay(500);

  lcd.setCursor(0, 1); // column, row
  lcd.print(" PINTU TERBUKA  ");
  digitalWrite(ledGreen, HIGH);

  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  delay(50);
  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  digitalWrite(relay, LOW);
  delay(5000);
  digitalWrite(relay, HIGH);
  delay(100);
  digitalWrite(ledGreen, LOW);

  lcd.setCursor(0, 1); // column, row
  lcd.print("  PINTU TERKUNCI   ");

  tone(buzzer, 2000);
  delay(100);
  noTone(buzzer);
  delay(50);
}

void invalid () {
  Serial.println("Access Denied");
  Serial.println();
  delay(500);

  lcd.setCursor(0, 1); // column, row
  lcd.print("Invalid RFID Tag");

  digitalWrite(ledRed, HIGH);
  tone(buzzer, 1500);
  delay(500);
  digitalWrite(ledRed, LOW);
  noTone(buzzer);
  delay(100);
  digitalWrite(ledRed, HIGH);
  tone(buzzer, 1500);
  delay(500);
  digitalWrite(ledRed, LOW);
  noTone(buzzer);
  delay(100);
  digitalWrite(ledRed, HIGH);
  tone(buzzer, 1500);
  delay(500);
  digitalWrite(ledRed, LOW);
  noTone(buzzer);

  lcd.setCursor(0, 1); // column, row
  lcd.print("   PINTU TERKUNCI   ");
}
