#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

// WiFiClient client;
const char *ssid = "ASUS";
const char *password = "air46664";
const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80
const long utcOffsetInSeconds = 25200;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);
#define tahun timeClient.getYear()
#define bulan timeClient.getMonth()
#define tanggal timeClient.getDate()
#define hari daysOfTheWeek[timeClient.getDay()]
#define jam timeClient.getHours()
#define menit timeClient.getMinutes()
#define detik timeClient.getSeconds()
const uint8_t fp[20] = { 0x38, 0x9E, 0x04, 0xD4, 0x3F, 0x2D, 0x2E, 0xEA, 0xDE, 0x98, 0x6B, 0x9C, 0x78, 0x61, 0x4B, 0xB5, 0xE2, 0x34, 0x80, 0xD8 };
char tanggalapi[11];


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  timeClient.begin();
  getjadwal();
}

void getjadwal() {
  timeClient.update();
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");

  if (bulan >= 10 && tanggal >= 10) {
    sprintf(tanggalapi, "%d/%d/%d", tahun, bulan, tanggal);
  } else if (bulan >= 10 && tanggal < 10) {
    sprintf(tanggalapi, "%d/%d/0%d", tahun, bulan, tanggal);
  } else if (bulan < 10 && tanggal >= 10) {
    sprintf(tanggalapi, "%d/0%d/%d", tahun, bulan, tanggal);
  } else if (bulan < 10 && tanggal < 10) {
    sprintf(tanggalapi, "%d/0%d/0%d", tahun, bulan, tanggal);
  }
  Serial.print(tanggalapi);
  Serial.print("\t");

  Serial.print(jam);
  Serial.print(":");
  Serial.print(menit);
  Serial.print(":");
  Serial.println(detik);
  Serial.println();

  // req_api();
  reqjadwal();
}
unsigned long lasttime = 0;
void loop() {
  // put your main code here, to run repeatedly:
}
void reqjadwal() {

  StaticJsonDocument<1024> doc;
  String url = "https://api.myquran.com/v1/sholat/jadwal/1225/";
  url += tanggalapi;
  WiFiClientSecure httpsClient;
  httpsClient.setFingerprint(fp);
  httpsClient.setTimeout(15000);  // 15 Seconds
  HTTPClient http;
  httpsClient.connect(url, httpsPort);
  http.begin(httpsClient, url);
  String payload;
  int code = http.GET();
  payload = http.getString();

  Serial.println("payload: ");
  Serial.println(payload);


  DeserializationError error = deserializeJson(doc, payload);
  JsonObject results = doc["data"]["jadwal"];
  String ashar = results["ashar"];
  String dhuha = results["dhuha"];
  String dzuhur = results["dzuhur"];
  String imsak = results["imsak"];
  String isya = results["isya"];
  String subuh = results["subuh"];
  Serial.print("Ashar = ");
  Serial.println(ashar);
  Serial.print("Dhuha = ");
  Serial.println(dhuha);
  Serial.print("Dzuhur = ");
  Serial.println(dzuhur);
  Serial.print("Imsak = ");
  Serial.println(imsak);
  Serial.print("Isya = ");
  Serial.println(isya);
  Serial.print("Subuh = ");
  Serial.println(subuh);
  Serial.println();

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    // return;
  }
  http.end();
  Serial.println("http ended");

  // if (httpsClient.verify(fp, url)) {
  //   http.begin(httpsClient, url);

  //   String payload;
  //   if (http.GET() == HTTP_CODE_OK)
  //     payload = http.getString();
  // } else {
  //   Serial.println("certificate doesn't match");
  // }


  // Serial.print("HTTPS Connecting");
  // int r = 0;  //retry counter
  // while ((!httpsClient.connect(url, httpsPort)) && (r < 30)) {
  //   delay(100);
  //   Serial.print(".");
  //   r++;
  // }
  // if (r == 30) {
  //   Serial.println("Connection failed");
  // } else {
  //   Serial.println("Connected to web");
  // }
}
