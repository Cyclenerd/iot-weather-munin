/*
  With this program you can read temperature and humidity from a DHT22 sensor.
  The data is sent via HTTP GET to an server.

  Tested with Arduino UNO and Ethernet Shield.

  More details: https://github.com/Cyclenerd/iot-weather-munin
*/

#include <DHT.h>;
#include <Ethernet.h>;
#include <SPI.h>;

/***************************************************************************************
   Configuration Section
 ***************************************************************************************/

// Weather Station Name (Location)
// At least 3 characters up to 15 characters a-zA-Z0-9
const char* name = "NAME";

// Server with weather.pl script
// http://<server_ip>/<script>
const byte server_ip[]   = { 192,168,1,123 };
const char* script = "/weather.pl";

// Report every n Minutes
const short unsigned int report_every_min = 1; // minutes

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x10 };
const byte ip[]  = { 192,168,1,32 }; // < ENTER YOUR IP ADDRESS HERE

/***************************************************************************************
   End Configuration Section
 ***************************************************************************************/

#define DHTPIN 7       // what pin we're connected to
#define DHTTYPE DHT22  // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

// Initialize the Ethernet client library
EthernetClient client;

/***************************************************************************************
   Helpers
 ***************************************************************************************/

// Pauses the program for the amount of time (in milliseconds)
const unsigned int sleep_time = report_every_min * 60 * 1000000; // min to ms
void sleep() {
  Serial.print("Wait for ");
  Serial.print(report_every_min);
  Serial.println(" minutes...");
  delay(sleep_time);
}

// The report function sends the data to the server
void report(double humidity, double temp_c) {
  // Build URL with parameters
  String script_get = "GET " + String(script) + "?w=" + String(name) + "&c=" +  String(temp_c) + "&h=" + String(humidity) + " HTTP/1.0";
  Serial.println(script_get);
  // Connect
  if (client.connect(server_ip, 80)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request:
    client.println(script_get);
    client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

/***************************************************************************************
   Let's start
 ***************************************************************************************/

// The setup function runs once when you press reset or power the board (after deep sleep)

void setup() {
  Serial.begin(115200);
  Serial.println();
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("IP address: ");
  Serial.println(Ethernet.localIP());
}

// The loop function runs over and over again
void loop() {
  // turn the LED on
  digitalWrite(LED_BUILTIN, HIGH);
  // Reading temperature or humidity takes about 250 ms!
  // Sensor readings may also be up to 2 seconds
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp_c = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp_c)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000); // sleep 2 seconds
    return;
  }

  // turn the LED off
  digitalWrite(LED_BUILTIN, LOW);
  // Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Temp: ");
  Serial.print(temp_c);
  Serial.println(" C");

  // Report via HTTP
  report(humidity, temp_c);
  
  // Wait
  sleep();
}
