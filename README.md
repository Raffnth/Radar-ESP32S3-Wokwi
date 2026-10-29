# Radar ESP32-S3 (Wokwi)

Simulasi radar mini dengan ESP32-S3, servo, sensor HC-SR04, dan OLED SSD1306. Servo menyapu 0-180°, sensor mengukur jarak di tiap sudut, dan OLED menampilkan jarum radar beserta dashboard sudut dan jarak.

## Komponen dan Pin

| Komponen | Pin komponen | Pin ESP32-S3 |
|---|---|---|
| OLED | SDA / SCL | GPIO 8 / GPIO 9 |
| Servo | PWM | GPIO 4 |
| HC-SR04 | TRIG / ECHO | GPIO 5 / GPIO 6 |

## Cara Menjalankan

1. Buka project Wokwi: wokwi.com/projects/479177463683247105
2. Klik **Start** dan tunggu compile.
3. Klik HC-SR04, lalu geser slider **Distance** (di bawah 100 cm) untuk memunculkan titik objek.

## Penulis

Raffi Anantha Setiawan - Politeknik Siber dan Sandi Negara
