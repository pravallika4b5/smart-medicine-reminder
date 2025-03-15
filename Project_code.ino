#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <Wire.h>
#include <U8g2lib.h>

// WiFi Credentials
#define WIFI_SSID "123456789"
#define WIFI_PASSWORD "pravallika4b5"

// Firebase Credentials
#define API_KEY "AIzaSyDHPyCzg56uSdqVlMFSoYZB6XQpARowJs0"
#define DATABASE_URL "https://smart-medicine-reminder-57610-default-rtdb.firebaseio.com/"

// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
bool signupOK = false;

// OLED Display (Change if needed)
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, D2, D1); 

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting...");

  // Initialize OLED
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  
  // WiFi Connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi!");
  Serial.println(WiFi.localIP());

  // Firebase Configuration
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Auth successful");
    signupOK = true;
  } else {
    Serial.printf("Firebase Auth failed: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK) {
    String reminderMessage;
    
    // Read the string from Firebase
    if (Firebase.RTDB.getString(&fbdo, "mainbucket/reminder")) {
      reminderMessage = fbdo.stringData();
      Serial.println("Received: " + reminderMessage);
    } else {
      Serial.println("Failed to get data: " + fbdo.errorReason());
      reminderMessage = "Error retrieving data";
    }

    // Display message on OLED
    u8g2.clearBuffer();
    u8g2.setCursor(0, 20);
    u8g2.print("Reminder:");
    u8g2.setCursor(0, 40);
    u8g2.print(reminderMessage);
    u8g2.sendBuffer();
  }

  delay(2000); // Update every 2 seconds
}
