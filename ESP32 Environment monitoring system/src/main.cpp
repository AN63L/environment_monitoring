// For BME680
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 bme; // I2C
// For SPI
// #define BME_SCK 18
// #define BME_MISO 19
// #define BME_MOSI 23
// #define BME_CS 5
// Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

// FOR PMS
#include <PMserial.h>
SerialPM pms(PMSx003, PMS_RX, PMS_TX); // PMSx003, RX, TX

// for onboard LED
#define LED 2

// For server
#include <WiFi.h>
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"

// Creds for WiFi
// https://stackoverflow.com/questions/62314497/access-of-outer-environment-variable-in-platformio
#define XSTR(x) #x
#define STR(x) XSTR(x)
const char *wifi_ssid = STR(WIFI_SSID);
const char *wifi_pwd = STR(WIFI_PWD);

// server setup
AsyncWebServer server(80);
AsyncEventSource events("/events");

// global variables for webpage
float temperature;
float pressure;
float humidity;
float gas_resistance;
float altitude;
char aqi;
float pm_1;
float pm_2;
float pm_10;

// 404 response
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

// processor for the HTML response - handles the update in variables
String processor(const String &var)
{
  if (var == "temperature")
  {
    return String(temperature);
  }
  else if (var == "pressure")
  {
    return String(pressure);
  }
  else if (var == "humidity")
  {
    return String(humidity);
  }
  else if (var == "gas_resistance")
  {
    return String(gas_resistance);
  }
  else if (var == "altitude")
  {
    return String(altitude);
  }
  else if (var == "pm_1")
  {
    return String(pm_1);
  }
  else if (var == "pm_2")
  {
    return String(pm_2);
  }
  else if (var == "pm_10")
  {
    return String(pm_10);
  }
  else
    return String("Error");
}

// main HTML
const char index_html[] PROGMEM = R"rawliteral(<!DOCTYPE HTML><html><body>
<h2>Temperature: <span id="temp">%temperature%</span>&deg;C</h2>
<h2>Pressure: <span id="press">%pressure%</span>hPa</h2>
<h2>Humidity: <span id="hum">%humidity%</span>&percnt;</h2>
<h2>Gas Resistance: <span id="gas">%gas_resistance%</span>&K&ohm;</h2>
<h2>Altitude: <span id="alti">%altitude%</span>m</h2>
<h2>PM 1.0: <span id="pm_1">%pm_1%</span>[ug/m3]</h2>
<h2>PM 2.5: <span id="pm_2">%pm_2%</span>[ug/m3]</h2>
<h2>PM 10: <span id="pm_10">%pm_10%</span>[ug/m3]</h2>
</body>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);

 source.addEventListener('pressure', function(e) {
  console.log("pressure", e.data);
  document.getElementById("press").innerHTML = e.data;
 }, false);

 source.addEventListener('humidity', function(e) {
  console.log("humidity", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);

 source.addEventListener('gas_resistance', function(e) {
  console.log("gas_resistance", e.data);
  document.getElementById("gas").innerHTML = e.data;
 }, false);

 source.addEventListener('altitude', function(e) {
  console.log("altitude", e.data);
  document.getElementById("alti").innerHTML = e.data;
 }, false);

 source.addEventListener('pm_1', function(e) {
  console.log("pm_1", e.data);
  document.getElementById("pm_1").innerHTML = e.data;
 }, false);

 source.addEventListener('pm_2', function(e) {
  console.log("pm_2", e.data);
  document.getElementById("pm_2").innerHTML = e.data;
 }, false);

 source.addEventListener('pm_10', function(e) {
  console.log("pm_10", e.data);
  document.getElementById("pm_10").innerHTML = e.data;
 }, false);
}
</script>

</html>)rawliteral";

void setup()
{
  Serial.begin(115200);
  // Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println(F("BME680 async test"));

  if (!bme.begin())
  {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1)
      ;
  }
  pms.init();

  delay(1000);
  // Set the device as a Station and Soft Access Point simultaneously
  WiFi.mode(WIFI_STA);
  Serial.println("wifi_ssid: ");
  Serial.println(wifi_ssid);
  Serial.println("wifi_pwd: ");
  Serial.println(wifi_pwd);
  WiFi.begin(wifi_ssid, wifi_pwd);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Define a route to serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println("ESP32 Web Server: New request received:"); // for debugging
              Serial.println("GET /");                                   // for debugging
              request->send_P(200, "text/html", index_html, processor);
              // request->send(200, "text/plain", "Hello World!");
            });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client)
                   {
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000); });

  // server handlers
  server.onNotFound(notFound);
  server.addHandler(&events);
  // Start the server
  server.begin();

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  pinMode(LED, OUTPUT);
}

void loop()
{
  Serial.println();
  Serial.println();

  digitalWrite(LED, HIGH);
  Serial.println("LED is on");
  Serial.println();

  Serial.print(F("BME 680 Reading started \n"));

  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();
  if (endTime == 0)
  {
    Serial.println(F("Failed to begin reading :("));
    return;
  }

  delay(50);
  if (!bme.endReading())
  {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  Serial.print(F("Reading completed \n"));

  // Serial.print(F("Temperature = "));
  // Serial.print(bme.temperature);
  // Serial.println(F(" *C"));
  temperature = bme.temperature;
  events.send(String(temperature).c_str(), "temperature", millis());

  // Serial.print(F("Pressure = "));
  // Serial.print(bme.pressure / 100.0);
  // Serial.println(F(" hPa"));
  pressure = bme.pressure / 100.0;
  events.send(String(pressure).c_str(), "pressure", millis());

  // Serial.print(F("Humidity = "));
  // Serial.print(bme.humidity);
  // Serial.println(F(" %"));
  humidity = bme.humidity;
  events.send(String(humidity).c_str(), "humidity", millis());

  // Serial.print(F("Gas = "));
  // Serial.print(bme.gas_resistance / 1000.0);
  // Serial.println(F(" KOhms"));
  gas_resistance = bme.gas_resistance / 1000.0;
  events.send(String(gas_resistance).c_str(), "gas_resistance", millis());

  // Serial.print(F("Approx. Altitude = "));
  // Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  // Serial.println(F(" m"));
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  events.send(String(gas_resistance).c_str(), "altitude", millis());

  Serial.print(F("PMS7003 Reading started \n"));
  pms.read(); // read the PM sensor
  // Serial.print(F("PM1.0 "));
  // Serial.print(pms.pm01);
  pm_1 = pms.pm01;
  events.send(String(pm_1).c_str(), "pm_1", millis());
  // Serial.print(F("\n"));
  // Serial.print(F("PM2.5 "));
  // Serial.print(pms.pm25);
  pm_2 = pms.pm25;
  events.send(String(pm_2).c_str(), "pm_2", millis());
  // Serial.print(F("\n"));
  // Serial.print(F("PM10 "));
  // Serial.print(pms.pm10);
  pm_10 = pms.pm10;
  events.send(String(pm_10).c_str(), "pm_10", millis());
  // Serial.println(F(" [ug/m3]"));
  Serial.print(F("PMS7003 Reading completed \n"));

  Serial.println();

  digitalWrite(LED, LOW);
  Serial.println("LED is off");
  delay(60000); // wait for 60 seconds
}