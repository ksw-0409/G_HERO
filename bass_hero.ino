#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>

// ----- NeoPixel 설정 -----
#define LED_PIN 6          // 네오픽셀 데이터 입력 핀
#define LED_COUNT 30       // 사용할 네오픽셀 LED 개수

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// ----- 피크 스위치 설정 -----
const int PEAK1 = 2;
bool status_peak1 = 0;
unsigned long lastDebounceTimePeak = 0;
const char PEAK_BIND = KEY_UP_ARROW;

// ----- 프렛 센서 설정 -----
const int inputPins[5] = {A0, A1, A2, A3, A10};     // 센서 핀
const char keyMap[5] = {'g', 'f', 'd', 's', 'a'};   // 키 매핑
const int analogThreshold = 700;                   // 아날로그 감도 기준
const int debounceDelay = 20;                      // 디바운스 시간 (ms)

bool lastStableState[5] = {false, false, false, false, false};
bool keyPressed[5] = {false, false, false, false, false};
unsigned long lastDebounceTime[5] = {0, 0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  pinMode(PEAK1, INPUT_PULLUP);
  pinMode(13, OUTPUT); // indicator LED
  Keyboard.begin();

  // 네오픽셀 초기화
  strip.begin();
  strip.show();  // LED 모두 끄고 시작
}

void loop() {
  // ----- 피크 버튼 처리 
  int sensorVal_peak1 = digitalRead(PEAK1);
  if (sensorVal_peak1 == HIGH) {
    if (status_peak1 == 1) {
      if ((millis() - lastDebounceTimePeak) > debounceDelay) {
        lastDebounceTimePeak = millis();
        status_peak1 = 0;
        digitalWrite(13, LOW);
        Keyboard.release(PEAK_BIND);
      }
    }
  } else {
    if (status_peak1 == 0) {
      if ((millis() - lastDebounceTimePeak) > debounceDelay) {
        lastDebounceTimePeak = millis();
        digitalWrite(13, HIGH);
        Keyboard.press(PEAK_BIND);
        status_peak1 = 1;
      }
    }
  }

  // ----- 프렛 입력 처리 -----
  for (int i = 0; i < 5; i++) {
    int val = analogRead(inputPins[i]);
    bool currentState = val >= analogThreshold;

    if (currentState != lastStableState[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (currentState != keyPressed[i]) {
        keyPressed[i] = currentState;
        if (keyPressed[i]) {
          Keyboard.press(keyMap[i]);
        } else {
          Keyboard.release(keyMap[i]);
        }
      }
    }

    lastStableState[i] = currentState;
  }

  // ----- 프렛 중 하나라도 눌렸는지 확인 -----
  bool anyFretPressed = false;
  for (int i = 0; i < 5; i++) { 
    if (keyPressed[i]) {
      anyFretPressed = true;
      break;
    }
  }

  // ----- 네오픽셀 상태 제어 -----
  if (anyFretPressed) {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); //빨
    }
  } else {
    for (int i = 0; i < LED_COUNT; i++) {
      strip.setPixelColor(i, 0); // 끄기
    }
  }
  strip.show();
}
