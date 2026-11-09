Alat yang digunakan : 
-ESP32
-DHT22 -> Sensor suhu
-Anemometer cup -> Sensor kecepatan angin

Langkah-langkah:
-Rakit sensor dan aktuator (jika ditambahkan) ke ESP32
-Koneksikan ESP32 ke laptop/PC
-Jalankan program monitoring di Arduino IDE
-Ambil data sebanyak yang anda perlukan (waktu dan frekuensi pengambilan data bisa disesuaikan)
-Jika sudah, simpan semua data yang suhu, kec angin, dan timestamp ke data base dan copy dengan format xlsx
-Jalankan program olah data randomforest.py
