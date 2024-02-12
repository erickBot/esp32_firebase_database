#include <WiFiProv.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiSTA.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

#include <Arduino.h>
#include <FirebaseESP32.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "PASACHE.CALLE-2.4G"
#define WIFI_PASSWORD "BC2817EP"

#define API_KEY "AIzaSyAprY6R3-GfG12UlhhkgXYpOiGZYMLgHVE"
#define DATABASE_URL "https://proyecto-bomba-agua-default-rtdb.firebaseio.com/"
#define USER_EMAIL "erickpasache0@gmail.com"
#define USER_PASSWORD "123456"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long readDataPrevMillis = 0;
unsigned long count = 0;
double power = 0;
double current = 0;
double frecuencia = 0;
double hour = 0;
String code = "OK";
bool status = false;
bool bomba = false;
bool resBomba = false;

void setup() 
  {

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);

}

void loop() {

  if (Firebase.ready() && (millis() - readDataPrevMillis > 1000 || readDataPrevMillis == 0)){
    readDataPrevMillis = millis();

    if (Firebase.getBool(fbdo, FPSTR("/Estacion1/write/bomba"))){
        
        resBomba = fbdo.boolData();

        if (resBomba){
          bomba = true;
        }else{
          bomba = false;
        }
        
    };

      if (bomba){
        //encender bomba

      }else{
        //apagar bomba

      }

      readDataFromBomba();
      sendDataToFirebase();

  }
  
}

void readDataFromBomba(){
  if (bomba){
    power = random(375, 380);
    current = random(85, 90);
    frecuencia = random(55, 60);
    //validar la lectura del variador, si todo es OK, status true
    code = "READY";
    status = true;
  }else{
    power = 0;
    current = 0;
    frecuencia = 0;
    count = 0;
     //validar 
    code = "OFF";
    status = false;
  }


}

void sendDataToFirebase(){
if (Firebase.ready() && (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    Serial.printf("Set int... %s\n", Firebase.setString(fbdo, F("/Estacion1/data/code"), code) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setDouble(fbdo, F("/Estacion1/data/current"), current) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setDouble(fbdo, F("/Estacion1/data/frecuencia"), frecuencia) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setDouble(fbdo, F("/Estacion1/data/hours"), count) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setDouble(fbdo, F("/Estacion1/data/power"), power) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set int... %s\n", Firebase.setBool(fbdo, F("/Estacion1/data/status"), status) ? "ok" : fbdo.errorReason().c_str());

    count ++;
  }
}
