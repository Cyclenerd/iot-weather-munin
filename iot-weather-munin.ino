/*
  With this program you can read temperature and humidity.
  The data is sent via HTTP GET to an server.

  =====================================================================================
  Optimized for battery operation. Device goes into Deep Sleep.
  According to the ESP8266 SDK, you can sleep max. for about ~71 minutes.
  =====================================================================================

  More details: https://github.com/Cyclenerd/iot-weather-munin
*/

#include <ESP8266WiFi.h>       // https://github.com/esp8266/Arduino
#include <ESP8266HTTPClient.h> // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266HTTPClient
#include <DHT.h>               // https://github.com/adafruit/DHT-sensor-library and
                               // https://github.com/adafruit/Adafruit_Sensor

/***************************************************************************************
   Configuration Section
 ***************************************************************************************/

// Weather Station Name (Location)
// At least 3 characters up to 15 characters a-zA-Z0-9
const char* name = "NAME";

// Server with weather.pl script
// http://<SERVER-IP>/weather.pl
const char* server = "http://SERVER-IP/weather.pl";

// WiFi SSID Name and Password
const char* ssid     = "SSID";
const char* password = "PASSWORD";

// Report every n Minutes
const short unsigned int report_every_min = 5; // minutes

/***************************************************************************************
   End Configuration Section
 ***************************************************************************************/


#define DHTPIN D6     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22 // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
HTTPClient http;

const unsigned int sleep_time = report_every_min * 60 * 1000000;
const short unsigned int max_connection_attempts = 50;
unsigned short int connection_attempts = 0;

/***************************************************************************************
   Helpers
 ***************************************************************************************/

// The connect function makes a connection with the WiFi and the HTTP server
void connect() {

  // Start connecting to WiFi network
  if (WiFi.status() != WL_CONNECTED) {
    // Check the maximum WiFi connection attempts
    if (connection_attempts > max_connection_attempts) {
      Serial.println("Maximum connection attempts for WiFi reached. Going to sleep now.");
      deep_sleep(); // Goodnight
    }

    Serial.print("Connecting to WiFi name ");
    Serial.println(ssid);

    // WiFi fix: https://github.com/esp8266/Arduino/issues/2186
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      if (WiFi.status() == WL_CONNECT_FAILED) {
        Serial.println("Failed to connect to WIFI. Please verify credentials!");
      }
      connection_attempts++; // Count connection attempts
      delay(500);
      Serial.print(".");
    }
    Serial.println();
  }
  // Connected to WiFi
  connection_attempts = 0; // Reset
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// The report function sends the data to the server
void report(double humidity, double temp_c, double temp_f, double heat_index_c, double heat_index_f) {
  // Build URL with parameters
  String server_url = String(server) + "?w=" + String(name) + "&c=" +  String(temp_c) + "&h=" + String(humidity);
  Serial.println(server_url);
  // Specify request destination
  http.begin(server_url);
  http.setTimeout(5000); // 5sec time out
  // set the username and password for the http request
  //http.setAuthorization(username, password);
  // Send the request
  int httpCode = http.GET();
  // httpCode will be negative on error
  if(httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("HTTP code: %d\n", httpCode);
  } else {
    Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

// The deep_sleep function sends the device into the deep sleep.
void deep_sleep() {
  Serial.print("Going into deep sleep for ");
  Serial.print(report_every_min);
  Serial.println(" minutes...");
  ESP.deepSleep(sleep_time);
}


/***************************************************************************************
   Let's start
 ***************************************************************************************/

// The setup function runs once when you press reset or power the board (after deep sleep)
void setup() {
  Serial.begin(115200);
  Serial.println();
  connect();
}

// The loop function runs over and over again
short unsigned int sensor_attempts = 0;
void loop() {
  // Check the maximum sensor reading attempts
  if (sensor_attempts >= 3) {
    deep_sleep();
  }

  // Reading temperature or humidity takes about 250 ms!
  // Sensor readings may also be up to 2 seconds
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp_c = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float temp_f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp_c) || isnan(temp_f)) {
    Serial.println("Failed to read from DHT sensor!");
    sensor_attempts++;
    delay(2000); // sleep 2 seconds
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float heat_index_f = dht.computeHeatIndex(temp_f, humidity);
  // Compute heat index in Celsius (isFahreheit = false)
  float heat_index_c = dht.computeHeatIndex(temp_c, humidity, false);

  report(humidity, temp_c, temp_f, heat_index_c, heat_index_f);

  deep_sleep();
}
