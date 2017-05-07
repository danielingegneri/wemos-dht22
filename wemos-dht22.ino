#include <DHT.h>

#include <ESP8266WiFi.h>

const char* ssid = "DANIEL-IOT";
const char* password = "passwordgoeshere";

#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
byte mac[6];

float hum;  //Stores humidity value
float temp; //Stores temperature value

// Time to sleep (in seconds):
const int sleepTimeS = 20;

// Host
const char* host = "dan.sh";

void setup()
{
  
  // Serial
  Serial.begin(115200);
  Serial.println("ESP8266 in normal mode");

  dht.begin();

  // Connect to WiFi
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  hum = dht.readHumidity();
  temp= dht.readTemperature();

  // TODO: Set a max retry count before skipping and going back to sleep!
  while(isnan(hum) || isnan(temp)) {
    Serial.println("Failed reading sensor. Trying again in 1 second...");
    delay(1000);
    hum = dht.readHumidity();
    temp= dht.readTemperature();
  }
  
  Serial.print("Humidity: ");
  Serial.println(hum);
  Serial.print("Temp: ");
  Serial.println(temp);

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Logging data to cloud
  Serial.print("Connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

WiFi.macAddress(mac);

  String macString = String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX);

  // This will send the request to the server
  client.print(String("GET /iot/record?temp=") + String(temp, 2) + "&hum=" + String(hum, 2) + "&mac=" + macString + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

  // Sleep
  Serial.println("ESP8266 in sleep mode");
  ESP.deepSleep(sleepTimeS * 1000000);

}

void loop()
{

}

