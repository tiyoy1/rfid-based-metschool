#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <driver/adc.h>

#define SS_PIN 21
#define RST_PIN 22
#define BUZZ_PIN 4
#define BUZZ_CHANNEL 2
#define DOOR_PIN 2
#define TERMINAL_NAME  "metschool"

// Google Apps Script web app URL
const char *mainLinkForSpr = "https://script.google.com/macros/s/AKfycby6BUOPgDOtukhDPbX-o-ed7bEsIUG-pwO__zMBtjYHC4do6hvQJkunvuwO4efjT0TO/exec";

//Masukin SSID Wi-Fi sama Password disini
const char *ssid = "Ite Missa Est";
const char *password = "eintrachtfrankfurt";

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::StatusCode status;

// Root certificate (for HTTPS)
const char *root_ca =
"-----BEGIN CERTIFICATE-----\n" \
"MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw\n" \
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n" \
"MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n" \
"MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n" \
"Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA\n" \
"A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo\n" \
"27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w\n" \
"Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw\n" \
"TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl\n" \
"qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH\n" \
"szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8\n" \
"Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk\n" \
"MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92\n" \
"wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p\n" \
"aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN\n" \
"VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID\n" \
"AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E\n" \
"FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb\n" \
"C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe\n" \
"QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy\n" \
"h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4\n" \
"7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J\n" \
"ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef\n" \
"MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/\n" \
"Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT\n" \
"6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ\n" \
"0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm\n" \
"2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb\n" \
"bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n" \
"-----END CERTIFICATE-----\n";

void beep(int count = 1) {
  
  for (size_t j = 0; j < count; j++) {
    if (j != 0) delay(300);
    for (int i = 200; i < 1000; i++) {
      ledcWrite(BUZZ_CHANNEL, i);
      delayMicroseconds(30);
    }
    ledcWrite(BUZZ_CHANNEL, 0);
  }
  pinMode(BUZZ_PIN, INPUT);
}

void openDoor() {
  digitalWrite(DOOR_PIN, HIGH);
  delay(2000);
  digitalWrite(DOOR_PIN, LOW);
}

void setup() {
  pinMode(DOOR_PIN, OUTPUT);
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("Connecting WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  beep(2);
  Serial.println("Ready to scan RFID...");
}

void handleDataFromGoogle(String data) {
  int colonIndex = data.indexOf(":");
  String accessType = data.substring(0, colonIndex);
  int nextColonIndex = data.indexOf(":", colonIndex + 1);
  String name = data.substring(colonIndex + 1, nextColonIndex);
  String text = data.substring(nextColonIndex + 1);

  Serial.println("Received from Google:");
  Serial.printf("Access Type: %s | Name: %s | Text: %s\n",
                accessType.c_str(), name.c_str(), text.c_str());

  if (accessType.equalsIgnoreCase("beep")) {
    beep(3);
  } else if (accessType.equalsIgnoreCase("door")) {
    openDoor();
  }
}

void getGoogleData() {
  HTTPClient http;
  String data;

  char url[200];
  int pointerShift = sprintf(url, "%s?uid=", mainLinkForSpr);

  for (size_t i = 0; i < mfrc522.uid.size; i++) {
    pointerShift += sprintf(url + pointerShift, "%X", mfrc522.uid.uidByte[i]);
  }

#ifdef TERMINAL_NAME
  pointerShift += sprintf(url + pointerShift, "&terminal=%s", TERMINAL_NAME);
#endif

  Serial.println(url);
  Serial.println("Connecting to Google...");

  http.begin(url, root_ca);
  const char *location = "Location";
  const char *headerKeys[] = {location};
  http.collectHeaders(headerKeys, 1);
  int code = http.GET();
  Serial.printf("HTTP code: %d\n", code);

  if (code == 302) {
    String newUrl = http.header(location);
    http.end();
    http.begin(newUrl, root_ca);
    code = http.GET();
    Serial.printf("Redirected HTTP code: %d\n", code);
    data = http.getString();
    Serial.println("Response: " + data);
  } else {
    Serial.println("Error: HTTP code " + String(code));
    if (code == 403 || code == -1) {
      Serial.println("Authorization issue — republish your Google script.");
    }
  }

  if (!data.isEmpty() && data.length() > 1) {
    handleDataFromGoogle(data);
  }

  http.end();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  beep();

  Serial.print("Scanned UID: ");
  for (size_t i = 0; i < mfrc522.uid.size; i++) {
    Serial.printf("%02X", mfrc522.uid.uidByte[i]);
  }
  Serial.println();

  getGoogleData();

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  beep();
}
