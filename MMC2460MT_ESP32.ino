#include <Wire.h>
#include <PubSubClient.h>
#include <WiFi.h>

// BMP280 I2C address is 0x76(108)
#define Addr 0x30

//Wifi Credentials
#define wifi_ssid "SSID"
#define wifi_password "Password"

//Define MQTT server and topics
#define mqtt_server "iot.eclipse.org"
#define X_topic "X_Axis"
#define Y_topic "Y_Axis"

WiFiClient espClient;
PubSubClient client;

volatile float X_top, Y_top;
void setup()
{
  Wire.begin(21, 22);
  Serial.begin(115200);
  Serial.println("RX Pin --->" + RX);
  Serial.println("TX Pin --->" + TX);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setClient(espClient);
}

//Wifi Setup
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//Reconnect
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(500);
    }
  }
}

void loop()
{
  MMC2460();
  delay(100);

  if (!client.connected()) {
    reconnect();
  }

  //Mentioned below directly executed in String url

  Serial.print("X Axis: ");
  Serial.println(String(X_top).c_str());
  client.publish(X_topic, String(X_top).c_str(), true);

  Serial.print("Y Axis: ");
  Serial.println(String(Y_top).c_str());
  client.publish(Y_topic, String(Y_top).c_str(), true);

  
  client.loop();
}

void MMC2460()
{
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select command register 0
  Wire.write(0x07);
  // Send measurement command
  Wire.write(0x23);
  // Stop I2C Transmission
  Wire.endTransmission();

  unsigned int data[4];
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select data register
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();

  // Request 4 byte of data
  Wire.requestFrom(Addr, 4);

  // xMag lsb, xMag msb, yMag lsb, yMag msb
  if (Wire.available() == 4)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    delay(100);
  }
  delay(300);

  // Convert the values
  int xMag = ((data[1] * 256) + data[0]);
  int yMag = ((data[3] * 256) + data[2]);

  // Output data to serial monitor
//  Serial.print("Magnetic field in X-Axis : ");
//  Serial.println(xMag);
//  Serial.print("Magnetic field in Y-Axis : ");
//  Serial.println(yMag);
  delay(1000);

  X_top = xMag;
  Y_top = yMag;
}
