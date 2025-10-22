#include <WiFi.h>
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "Uma-2.4G";
const char* password = "Siva@infy21";

// PubNub keys
String pubKey = "pub-c-daf4b9aa-8030-4420-97e0-e03abc8dac99";
String subKey = "sub-c-874e74cd-ed21-4746-b075-50ff52c124a7";
String channel = "bus_tracker";

// Dummy route: Tambaram → Sholinganallur (approximate points along the route)
float route[][2] = {
  {12.9229, 80.1275},  // Tambaram
  {12.9255, 80.1352},
  {12.9318, 80.1490},  // Medavakkam
  {12.9392, 80.1578},
  {12.9476, 80.1685},  // Pallikaranai
  {12.9528, 80.1794},
  {12.9602, 80.1880},  // Perungudi
  {12.9640, 80.1960},
  {12.9678, 80.2065},  // Sholinganallur Signal
  {12.8985, 80.2200}   // Sholinganallur (End)
};

int totalPoints = sizeof(route) / sizeof(route[0]);
int currentPoint = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    float lat = route[currentPoint][0];
    float lon = route[currentPoint][1];
    sendToPubNub(lat, lon);

    currentPoint++;
    if (currentPoint >= totalPoints) {
      currentPoint = 0; // Restart the route loop
    }
  }
  delay(5000); // Send location every 5 seconds
}

void sendToPubNub(float lat, float lon) {
  HTTPClient http;
  String url = "http://ps.pndsn.com/publish/" + pubKey + "/" + subKey +
               "/0/" + channel + "/0/%7B%22lat%22%3A" + String(lat, 6) +
               "%2C%22lon%22%3A" + String(lon, 6) + "%7D";

  http.begin(url);
  int httpCode = http.GET();
  http.end();

  Serial.println("Location sent: " + String(lat, 6) + "," + String(lon, 6));
}
