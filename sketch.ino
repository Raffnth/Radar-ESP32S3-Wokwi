#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Pin ESP32-S3 ----------
#define SDA_PIN    8
#define SCL_PIN    9
#define SERVO_PIN  4
#define TRIG_PIN   5
#define ECHO_PIN   6

// ---------- Servo via LEDC ----------
#define SERVO_FREQ   50
#define SERVO_BITS   14
#define SERVO_MIN_US 500
#define SERVO_MAX_US 2400

void servoWrite(int deg) {
  deg = constrain(deg, 0, 180);
  uint32_t us = map(deg, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  uint32_t duty = (uint32_t)((uint64_t)us * ((1 << SERVO_BITS) - 1) / 20000UL);
  ledcWrite(SERVO_PIN, duty);
}

// ---------- OLED ----------
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// ---------- Radar ----------
const int CX = 32;
const int CY = 58;
const int R  = 30;
const int MAX_DIST = 100;
const int STEP = 3;
const int N = 180 / STEP + 1;

int distMap[N];
int angle = 0;
int dir = STEP;
float curDist = -1;

// ---------- Baca jarak HC-SR04 (cm) ----------
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long dur = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (dur == 0) return -1;
  return dur * 0.0343 / 2.0;
}

// ---------- Gambar radar + dashboard ----------
void drawScreen() {
  display.clearDisplay();

  // Area radar (kiri)
  display.drawLine(CX - R, CY, CX + R, CY, SSD1306_WHITE);
  for (int a = 0; a <= 180; a += 4) {
    float r1 = a * DEG_TO_RAD;
    display.drawPixel(CX + R * cos(r1), CY - R * sin(r1), SSD1306_WHITE);
    display.drawPixel(CX + (R / 2) * cos(r1), CY - (R / 2) * sin(r1), SSD1306_WHITE);
  }

  // Titik objek hasil scan
  for (int i = 0; i < N; i++) {
    if (distMap[i] > 0 && distMap[i] <= MAX_DIST) {
      float r2 = (i * STEP) * DEG_TO_RAD;
      float rr = (float)distMap[i] / MAX_DIST * R;
      display.fillCircle(CX + rr * cos(r2), CY - rr * sin(r2), 1, SSD1306_WHITE);
    }
  }

  // Jarum radar
  float rad = angle * DEG_TO_RAD;
  display.drawLine(CX, CY, CX + R * cos(rad), CY - R * sin(rad), SSD1306_WHITE);

  // Pemisah
  display.drawLine(66, 0, 66, 63, SSD1306_WHITE);

  // Dashboard (kanan)
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(70, 0);
  display.print("SUDUT");
  display.setTextSize(2);
  display.setCursor(70, 10);
  display.print(angle);
  display.setTextSize(1);
  display.print((char)247);

  display.setTextSize(1);
  display.setCursor(70, 30);
  display.print("JARAK");
  display.setTextSize(2);
  display.setCursor(70, 40);
  if (curDist < 0) display.print("---");
  else display.print((int)curDist);
  display.setTextSize(1);
  display.print("cm");

  display.setCursor(70, 57);
  if (curDist > 0 && curDist <= MAX_DIST) display.print("OBJEK!");
  else display.print("AMAN");

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("1. Mulai");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Wire.begin(SDA_PIN, SCL_PIN);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED GAGAL init");
    while (true) delay(10);
  }
  Serial.println("2. OLED OK");

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(20, 15);
  display.println("RADAR");
  display.setTextSize(1);
  display.setCursor(15, 40);
  display.println("ESP32-S3 + HC-SR04");
  display.display();

  Serial.println("3. Init servo (LEDC)...");
  ledcAttach(SERVO_PIN, SERVO_FREQ, SERVO_BITS);
  servoWrite(0);
  Serial.println("4. Servo OK");

  for (int i = 0; i < N; i++) distMap[i] = -1;
  delay(1500);
}

void loop() {
  servoWrite(angle);
  delay(40);

  curDist = readDistance();
  distMap[angle / STEP] = (curDist > 0 && curDist <= MAX_DIST) ? (int)curDist : -1;

  drawScreen();

  Serial.printf("Sudut: %3d deg | Jarak: ", angle);
  if (curDist < 0) Serial.println("---");
  else Serial.printf("%.1f cm\n", curDist);

  angle += dir;
  if (angle >= 180) { angle = 180; dir = -STEP; }
  if (angle <= 0)   { angle = 0;   dir =  STEP; }
}