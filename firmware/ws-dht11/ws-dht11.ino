/*
 *  Weather Station App
 *  CGS Semester 2
 *  Task 2
 *  Author: your name here
 */

#include <math.h>
#include <WiFi.h>
#include <aREST.h>
#include <DHT.h>

// DHT11 sensor pins
#define DHTPIN 26
#define DHTTYPE DHT11

// Raindrop sensor pin
#define RAIN_SENSOR_PIN 33

// Create aREST instance
aREST rest = aREST();

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

// WiFi parameters
//const char* ssid = "Proxima";
//const char* password = "centauri";

const char* ssid = "liberator1";
const char* password = "7i@klv?CwRY!";


//Static IP address configuration
// P connections
#define LISTEN_PORT 80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API
float temperature;
float humidity;
char* location = "Death Star";
int timer = 72000;
int rainStatus = 0; // Initialize rain status to 0 (not raining)

// Declare functions to be exposed to the API
int ledControl(String command);

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // Init DHT
  dht.begin();

  // Init variables and expose them to REST API
  rest.variable("temperature", &temperature);
  rest.variable("humidity", &humidity);
  rest.variable("location", &location);
  rest.variable("rain", &rainStatus); // Expose rain status

  // Give name and ID to device
  rest.set_id("116");
  rest.set_name("Chinese_Listening_Device");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 1, 116); // set static IP
  IPAddress gateway(192, 168, 1, 1); // set gateway
  Serial.print(F("Setting static IP to : "));
  Serial.println(ip);
  IPAddress subnet(255, 255, 255, 0); // set subnet
  WiFi.config(ip, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Reading temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Read raindrop sensor
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  // You may need to adjust the threshold value based on your sensor and conditions
  if (rainValue == 4095)
  {
    rainStatus = 0; // It's  NOT raining
  }
  else
  {
    rainStatus = 1; // It's raining
  }

  // Prints the temperature in Celsius, humidity, and rain status
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Rain Status: ");
  Serial.println(rainStatus);
  Serial.print("Rain value: ");
  Serial.println(rainValue);
  Serial.print("Timer: ");
  Serial.println(timer);
  delay(5000);
  timer--;

  // Check running time and reset if expired
  if (timer == 0)
  {
    delay(3000);
    Serial.println("Resetting..");
    ESP.restart();
  }

  // Handle REST calls
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }
  while (!client.available())
  {
    delay(1);
  }
  rest.handle(client);
  // Serial.println("called");
}

// Custom function accessible by the API
int ledControl(String command)
{
  // Get state from command
  int state = command.toInt();

  digitalWrite(6, state);
  return 1;
}
