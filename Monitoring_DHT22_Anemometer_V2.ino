#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// =============================
// Konfigurasi WiFi
// =============================
const char* ssid = "";
const char* password = "";

// =============================
// URL Google Apps Script
// =============================
const char* serverName = "";

// =============================
// DHT dan Anemometer
// =============================
#define DHTPIN 14
#define DHTTYPE DHT22
#define WIND_SENSOR_PIN 4
#define READ_TIME 1000
#define WIND_SPEED_18_PULSE_SECOND 2.0
#define ONE_ROTATION_SENSOR 18.0

DHT dht(DHTPIN, DHTTYPE);
volatile unsigned long Rotations = 0;
float WindSpeed;
float Temperature;
unsigned long lastSendTime = 0;
unsigned long gulStart_Read_Timer = 0;

// ISR untuk anemometer
void IRAM_ATTR isr_rotation() {
  Rotations++;
}

void setup() {
  Serial.begin(115200);
  pinMode(WIND_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WIND_SENSOR_PIN), isr_rotation, CHANGE);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi!");
}

void loop() {
  // Baca sensor tiap 1 detik
  if (millis() - gulStart_Read_Timer >= READ_TIME) {
    noInterrupts();
    WindSpeed = WIND_SPEED_18_PULSE_SECOND / ONE_ROTATION_SENSOR * (float)Rotations / 10.0;
    Temperature = dht.readTemperature();
    interrupts();
    Rotations = 0;
    gulStart_Read_Timer = millis();
  }

  // Kirim data ke Google Sheets setiap 1 menit
  if (millis() - lastSendTime >= 60000 && WiFi.status() == WL_CONNECTED) {
    sendToGoogleSheets(Temperature, WindSpeed);
    lastSendTime = millis();
  }
}

void sendToGoogleSheets(float suhu, float angin) {
  if (isnan(suhu)) {
    Serial.println("Gagal membaca suhu!");
    return;
  }

  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/json");

  String jsonData = "{\"suhu\": " + String(suhu, 2) + ", \"angin\": " + String(angin, 2) + "}";
  int httpResponseCode = http.POST(jsonData);

  if (httpResponseCode > 0) {
    Serial.print("Data terkirim ke Google Sheets: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Gagal mengirim data. Kode error: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}
