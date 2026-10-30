#define TINY_GSM_MODEM_SIM7600
#define DEBUG true

#include <TinyGsmClient.h>
#include <PubSubClient.h>

// GPRS & MQTT Settings
const char broker[] = "test.mosquitto.org";
const int brokerPort = 1883;
const char topicTracking[] = "sim7600/nmea";
const char apn[] = "airtelgprs.com";

// Modem control pins
#define LTE_PWRKEY_PIN 5
#define LTE_RESET_PIN 6
#define LTE_FLIGHT_PIN 7

TinyGsm modem(Serial1);
TinyGsmClient gsmClient(modem);
PubSubClient mqttClient(gsmClient);

unsigned long lastTrackSendTime = 0;
const unsigned long TRACK_SEND_INTERVAL = 1500;

String sendData(const String &command, const int timeout, boolean debug = false) {
  String response = "";
  Serial1.println(command);
  long int time = millis();
  while (millis() - time < timeout) {
    while (Serial1.available()) {
      char c = Serial1.read();
      response += c;
    }
  }
  if (debug) SerialUSB.println("Response: " + response);
  return response;
}

bool setupGPRS() {
  if (modem.isGprsConnected()) return true;
  return modem.gprsConnect(apn, "", "");
}

bool connectMQTT() {
  if (!modem.isGprsConnected()) setupGPRS();
  String clientId = "Client-" + String(random(0xffff), HEX);
  bool connected = mqttClient.connect(clientId.c_str());
  SerialUSB.println(connected ? "MQTT connected" : "MQTT connection failed");
  return connected;
}

void maintainConnection() {
  if (!mqttClient.connected()) connectMQTT();
}

void publishMessage(const char* topic, const String& payload) {
  maintainConnection();
  SerialUSB.println("Publishing to topic: " + String(topic));
  SerialUSB.println("Payload: " + payload);
  if (mqttClient.publish(topic, payload.c_str())) {
    SerialUSB.println("✅ Payload sent successfully");
  } else {
    SerialUSB.println("❌ Failed to send payload");
  }
}

void sendTrackData() {
  String response = sendData("AT+CGPSINFO", 2000, DEBUG);
  if (response.indexOf("+CGPSINFO:") == -1 || response.indexOf(",,,,,,") != -1) {
    SerialUSB.println("GPS not ready or invalid data");
    return;
  }

  int start = response.indexOf(":") + 1;
  String data = response.substring(start);
  data.trim();

  int latEnd = data.indexOf(',', 0);
  String latRaw = data.substring(0, latEnd);
  int latDirEnd = data.indexOf(',', latEnd + 1);
  String latDir = data.substring(latEnd + 1, latDirEnd);

  int lonStart = latDirEnd + 1;
  int lonEnd = data.indexOf(',', lonStart);
  String lonRaw = data.substring(lonStart, lonEnd);
  int lonDirEnd = data.indexOf(',', lonEnd + 1);
  String lonDir = data.substring(lonEnd + 1, lonDirEnd);

  if (latRaw.length() < 4 || lonRaw.length() < 5) {
    SerialUSB.println("Invalid coordinates");
    return;
  }

  double lat = latRaw.toDouble();
  double lon = lonRaw.toDouble();
  double latDeg = int(lat / 100);
  double latMin = lat - (latDeg * 100);
  double latDec = latDeg + (latMin / 60);
  if (latDir == "S") latDec = -latDec;

  double lonDeg = int(lon / 100);
  double lonMin = lon - (lonDeg * 100);
  double lonDec = lonDeg + (lonMin / 60);
  if (lonDir == "W") lonDec = -lonDec;

  String payload = "{";
  payload += "\"carId\":1,";
  payload += "\"latitude\":" + String(latDec, 6) + ",";
  payload += "\"longitude\":" + String(lonDec, 6);
  payload += "}";

  publishMessage(topicTracking, payload);
}

void initModem() {
  // Initialize modem control pins
  pinMode(LTE_RESET_PIN, OUTPUT);
  digitalWrite(LTE_RESET_PIN, LOW);
  pinMode(LTE_PWRKEY_PIN, OUTPUT);
  digitalWrite(LTE_PWRKEY_PIN, LOW);
  delay(100);
  digitalWrite(LTE_PWRKEY_PIN, HIGH);
  delay(2000);
  digitalWrite(LTE_PWRKEY_PIN, LOW);
  pinMode(LTE_FLIGHT_PIN, OUTPUT);
  digitalWrite(LTE_FLIGHT_PIN, LOW);

  modem.restart();
  delay(3000);
  setupGPRS();

  // Turn on CGPS
  sendData("AT+CGPS=0", 1000, DEBUG);
  delay(500);
  sendData("AT+CGPS=1", 1000, DEBUG);
  delay(2000);
  SerialUSB.println("Modem initialized and CGPS started");
}

void setup() {
  SerialUSB.begin(115200);
  Serial1.begin(115200);
  randomSeed(analogRead(0));

  SerialUSB.println("Initializing modem...");
  initModem();
  mqttClient.setServer(broker, brokerPort);
  SerialUSB.println("Setup complete");
}

void loop() {
  mqttClient.loop();
  maintainConnection();

  if (millis() - lastTrackSendTime >= TRACK_SEND_INTERVAL) {
    lastTrackSendTime = millis();
    sendTrackData();
  }
}
