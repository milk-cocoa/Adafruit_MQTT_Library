/***************************************************
  Adafruit MQTT Library ESP8266 Example
  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "your_SSID"
#define WLAN_PASS       "your_password"

/************************* Milkcocoa Setup *********************************/

#define MILKCOCOA_SERVER      "your_milkcocoa_app_id.mlkcca.com"
#define MILKCOCOA_SERVERPORT  1883
#define MILKCOCOA_USERNAME    "sdammy"
#define MILKCOCOA_KEY         "your_milkcocoa_app_id"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Store the MQTT server, client ID, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = MILKCOCOA_SERVER;
// Set a unique MQTT client ID using the Milkcocoa key + the date and time the sketch
// was compiled (so this should be unique across multiple devices for a user,
// alternatively you can manually set this to a GUID or other random value).
const char MQTT_CLIENTID[] PROGMEM  = __TIME__ MILKCOCOA_USERNAME;
const char MQTT_USERNAME[] PROGMEM  = MILKCOCOA_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = MILKCOCOA_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MILKCOCOA_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Topic ***************************************/

// Setup a pubulisher & subscriber.
// Topic Name: "your_milkcocoa_app_id/your_milkcocoa_datastore_name/method_name"
const char PUSH_TOPIC[] PROGMEM = "your_milkcocoa_app_id/your_milkcocoa_datastore_name/push";
const char SEND_TOPIC[] PROGMEM = "your_milkcocoa_app_id/your_milkcocoa_datastore_name/send";
Adafruit_MQTT_Publish pushPublisher = Adafruit_MQTT_Publish(&mqtt, PUSH_TOPIC);
Adafruit_MQTT_Subscribe pushSubscriber = Adafruit_MQTT_Subscribe(&mqtt, PUSH_TOPIC);

/*************************** Sketch Code ************************************/

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&pushSubscriber);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
 Adafruit_MQTT_Subscribe *subscription;
 while ((subscription = mqtt.readSubscription(1000))) {
   if (subscription == &pushSubscriber) {
     Serial.print(F("Got: "));
     Serial.println((char *)pushSubscriber.lastread);
   }
 }

  // publish
  // e.g. publish("{ \"params\":{ \"your_key1\":\"your_value1\", \"your_key2\":\"your_value2\",...} }");
  pushPublisher.publish("{\"params\":{\"content\":\"from ESP8266!\"}}");

  delay(1000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println(ret);
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}