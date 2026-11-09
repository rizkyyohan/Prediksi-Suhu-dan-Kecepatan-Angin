#include <DHT.h>

// =============================
// Konstanta Anemometer
// =============================
#define READ_TIME 1000 // ms
#define WIND_SENSOR_PIN 4 // pin anemometer (D4)
#define WIND_SPEED_18_PULSE_SECOND 2.0 // kecepatan angin untuk 18 pulsa per detik (bisa dikalibrasi)
#define ONE_ROTATION_SENSOR 18.0 // jumlah pulsa per satu rotasi

// =============================
// Konstanta DHT22
// =============================
#define DHTPIN 14      // pin data sensor DHT22
#define DHTTYPE DHT22  // tipe sensor DHT22
DHT dht(DHTPIN, DHTTYPE);

// =============================
// Variabel global
// =============================
volatile unsigned long Rotations = 0; // counter untuk jumlah rotasi (dihitung via interrupt)
float WindSpeed; // hasil kecepatan angin (m/s)
float Temperature; // hasil pembacaan suhu (°C)
unsigned long gulStart_Read_Timer = 0;

// =============================
// Setup
// =============================
void setup() {
  Serial.begin(9600);
  pinMode(WIND_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WIND_SENSOR_PIN), isr_rotation, CHANGE); // interrupt untuk deteksi pulsa

  dht.begin(); // inisialisasi DHT22

  Serial.println("Monitoring Kecepatan Angin & Suhu");
  Serial.println("----------------------------------");

  sei(); // aktifkan interrupt
  gulStart_Read_Timer = millis();
}

// =============================
// Loop utama
// =============================
void loop() {
  if ((millis() - gulStart_Read_Timer) >= READ_TIME) {
    cli(); // nonaktifkan interrupt saat perhitungan

    // Hitung kecepatan angin
    WindSpeed = WIND_SPEED_18_PULSE_SECOND / ONE_ROTATION_SENSOR * (float)Rotations / 10.0;

    // Baca suhu dari DHT22
    Temperature = dht.readTemperature(); // hanya suhu, tanpa kelembapan

    // Cek apakah pembacaan suhu valid
    if (isnan(Temperature)) {
      Serial.println("Gagal membaca suhu dari DHT22!");
    } else {
      Serial.print("Kecepatan Angin (m/s): ");
      Serial.print(WindSpeed, 2);
      Serial.print("\tSuhu (°C): ");
      Serial.println(Temperature, 2);
    }

    sei(); // aktifkan kembali interrupt
    Rotations = 0; // reset counter rotasi
    gulStart_Read_Timer = millis();
  }
}

// =============================
// ISR (Interrupt Service Routine)
// =============================
void isr_rotation() {
  Rotations++;
}
