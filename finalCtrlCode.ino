#include <Wire.h>
#include <Keyboard.h>
#include "Adafruit_MPR121.h"
#include "HID.h"

// ============================================================
// Leonardo HID mouse with real horizontal scroll / pan
// 기본 Mouse.h는 세로 wheel만 지원해서, horizontal pan이 들어간
// custom HID report를 직접 추가한다.
// ============================================================
#define ADV_MOUSE_REPORT_ID 4

static const uint8_t _advancedMouseHIDDescriptor[] PROGMEM = {
  0x05, 0x01,                    // Usage Page (Generic Desktop)
  0x09, 0x02,                    // Usage (Mouse)
  0xA1, 0x01,                    // Collection (Application)
  0x85, ADV_MOUSE_REPORT_ID,     //   Report ID
  0x09, 0x01,                    //   Usage (Pointer)
  0xA1, 0x00,                    //   Collection (Physical)

  0x05, 0x09,                    //     Usage Page (Button)
  0x19, 0x01,                    //     Usage Minimum (Button 1)
  0x29, 0x03,                    //     Usage Maximum (Button 3)
  0x15, 0x00,                    //     Logical Minimum (0)
  0x25, 0x01,                    //     Logical Maximum (1)
  0x95, 0x03,                    //     Report Count (3)
  0x75, 0x01,                    //     Report Size (1)
  0x81, 0x02,                    //     Input (Data, Variable, Absolute)
  0x95, 0x01,                    //     Report Count (1)
  0x75, 0x05,                    //     Report Size (5)
  0x81, 0x03,                    //     Input (Constant, Variable, Absolute) padding

  0x05, 0x01,                    //     Usage Page (Generic Desktop)
  0x09, 0x30,                    //     Usage (X)
  0x09, 0x31,                    //     Usage (Y)
  0x09, 0x38,                    //     Usage (Wheel)
  0x15, 0x81,                    //     Logical Minimum (-127)
  0x25, 0x7F,                    //     Logical Maximum (127)
  0x75, 0x08,                    //     Report Size (8)
  0x95, 0x03,                    //     Report Count (3)
  0x81, 0x06,                    //     Input (Data, Variable, Relative)

  0x05, 0x0C,                    //     Usage Page (Consumer)
  0x0A, 0x38, 0x02,              //     Usage (AC Pan) = horizontal scroll
  0x15, 0x81,                    //     Logical Minimum (-127)
  0x25, 0x7F,                    //     Logical Maximum (127)
  0x75, 0x08,                    //     Report Size (8)
  0x95, 0x01,                    //     Report Count (1)
  0x81, 0x06,                    //     Input (Data, Variable, Relative)

  0xC0,                          //   End Collection
  0xC0                           // End Collection
};

typedef struct {
  uint8_t buttons;
  int8_t x;
  int8_t y;
  int8_t wheel;
  int8_t hWheel;
} AdvancedMouseReport;

class AdvancedMouse_ {
public:
  AdvancedMouse_() {
    static HIDSubDescriptor node(_advancedMouseHIDDescriptor, sizeof(_advancedMouseHIDDescriptor));
    HID().AppendDescriptor(&node);
  }

  void begin() {}
  void end() {}

  void move(int x, int y, int wheel, int hWheel) {
    AdvancedMouseReport report;
    report.buttons = 0;
    report.x = clampToInt8(x);
    report.y = clampToInt8(y);
    report.wheel = clampToInt8(wheel);
    report.hWheel = clampToInt8(hWheel);
    HID().SendReport(ADV_MOUSE_REPORT_ID, &report, sizeof(report));
  }

private:
  int8_t clampToInt8(int v) {
    if (v > 127) return 127;
    if (v < -127) return -127;
    return (int8_t)v;
  }
};

AdvancedMouse_ AdvancedMouse;

// ===================== PIN MAP =====================
const int LED_PIN     = 10;    // PWM pin
const int BUTTON_PIN  = 14;    // Shared button: single click / double click

// 수정: 실제 조이스틱 X/Y가 반대로 들어와서 A0/A1을 교환
const int STICK_X_PIN = A0;
const int STICK_Y_PIN = A1;

const int ENCODER_A   = 5;
const int ENCODER_B   = 6;
const int MPR121_INT  = 4;     // Optional. Current code polls filteredData().

// ===================== USER SETTINGS =====================
const bool DEBUG_SERIAL = false;

// One shared button behavior
const unsigned long DEBOUNCE_MS      = 25;
const unsigned long DOUBLE_CLICK_MS  = 260;
const unsigned long LONG_PRESS_MS    = 5000;
const bool ENABLE_LONG_PRESS_RESET   = false; // true로 바꾸면 5초 길게 눌렀을 때 리셋

// Joystick behavior
const int JOY_CENTER_X = 512;
const int JOY_CENTER_Y = 512;
const int JOY_DEADZONE = 130;
const int MOUSE_DIVISOR = 45;      // 작을수록 마우스 포인터가 빠름
const int MOUSE_MAX_STEP = 8;
const unsigned long JOY_INTERVAL_MS = 15;

// X/Y 핀을 바꾼 상태 기준. 방향만 반대면 여기만 true로 바꾸면 됨.
const bool INVERT_MOUSE_X = true;
const bool INVERT_MOUSE_Y = false;
const bool INVERT_ARROW_X = true;
const bool INVERT_ARROW_Y = false;

// Encoder scroll behavior
const int ENCODER_COUNTS_PER_SCROLL = 2; // 너무 민감하면 2 또는 4로 변경
const int ENCODER_SCROLL_STEP = 1;
const bool REVERSE_ENCODER_SCROLL = false;

// MPR121 slider behavior
const unsigned long MPR_INTERVAL_MS = 35;
const uint8_t MPR_CYCLES_PER_DECISION = 4;
// 전극별 입력 민감도.
// 각 전극에서 filteredData가 직전값보다 이 값 이상 감소하면 해당 전극 입력으로 본다.
// 값이 작을수록 민감하고, 값이 클수록 둔감하다.
// index: 0~5 = 가로 슬라이더, 6~11 = 세로 슬라이더
const uint16_t MPR_MIN_DROP[12] = {
  1, 1, 2, 2, 1, 1,   // electrode 0~5: horizontal slider
  2, 2, 2, 1, 2, 2    // electrode 6~11: vertical slider
};
// 피크 index 대신, 입력 분포의 무게중심(center of mass)이 얼마나 이동했는지 본다.
// 단위는 electrode pitch. 예: 0.7이면 전극 간격의 70% 이상 중심이 이동해야 스크롤.
const float MPR_MIN_CENTER_MOVE = 0.7;

// 한 decision window에서 이 값 이상의 총 입력량이 있어야 active로 본다.
// 너무 튀면 4~8 정도로 올리고, 너무 둔하면 1~2로 낮춘다.
const uint16_t MPR_MIN_TOTAL_DROP = 3;

const uint8_t MPR_INACTIVE_WINDOWS_TO_RESET = 2;
const int MPR_SCROLL_STEP = 1;
const bool REVERSE_H_SLIDER = true;   // 0->5 방향이 반대처럼 느껴지면 true
const bool REVERSE_V_SLIDER = false;   // 6->11 방향이 반대처럼 느껴지면 true

// HID horizontal scroll behavior
const bool REVERSE_HID_HORIZONTAL_SCROLL = false; // 가로 스크롤 방향만 반대면 true

// LED behavior
const int LED_IDLE_BRIGHTNESS = 3;    // 평소 중간 밝기
const int LED_BLINK_BRIGHTNESS = 5;
const unsigned long LED_BLINK_INTERVAL_MS = 120;

// ===================== GLOBALS =====================
Adafruit_MPR121 cap = Adafruit_MPR121();
bool mprOK = false;

// Modes
bool encoderHorizontalMode = false; // false: vertical scroll, true: horizontal scroll
bool joystickMouseMode = false;     // false: arrow keys, true: mouse pointer

// Encoder state
long encoderCount = 0;
long lastEncoderCountForScroll = 0;
int lastEncoded = 0;
int encoderScrollAccum = 0;

// Button state
bool stableButtonPressed = false;
bool lastStableButtonPressed = false;
bool lastRawButtonPressed = false;
unsigned long lastDebounceTime = 0;
unsigned long buttonPressStart = 0;
bool longPressFired = false;
uint8_t pendingClickCount = 0;
unsigned long firstClickTime = 0;

// MPR state
uint16_t prevMpr[12];

// 각 전극이 몇 번 눌렸는지가 아니라, filteredData가 얼마나 감소했는지를 누적한다.
// 그래야 peak 한 칸 이동보다 더 연속적인 slider 이동을 볼 수 있다.
uint16_t hAccum[6] = {0, 0, 0, 0, 0, 0};
uint16_t vAccum[6] = {0, 0, 0, 0, 0, 0};
uint8_t mprCycleCount = 0;

float hCenterAnchor = -1.0;
float vCenterAnchor = -1.0;
uint8_t hInactiveWindows = 0;
uint8_t vInactiveWindows = 0;
unsigned long lastMprRead = 0;

// Joystick/key state
unsigned long lastJoyUpdate = 0;
bool leftPressed = false;
bool rightPressed = false;
bool upPressed = false;
bool downPressed = false;

// LED blink state
int ledBlinkTransitionsLeft = 0;
bool ledBlinkOn = false;
unsigned long lastLedBlinkTime = 0;

void(* resetFunc)(void) = 0;

// ===================== SETUP =====================
void setup() {
  if (DEBUG_SERIAL) Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  pinMode(MPR121_INT, INPUT_PULLUP);

  analogWrite(LED_PIN, LED_IDLE_BRIGHTNESS);

  Wire.begin();
  Keyboard.begin();
  AdvancedMouse.begin();

  int A = digitalRead(ENCODER_A);
  int B = digitalRead(ENCODER_B);
  lastEncoded = (A << 1) | B;

  setupMPR121();

  // MPR 초기 기준값 저장
  if (mprOK) {
    delay(50);
    for (uint8_t i = 0; i < 12; i++) {
      prevMpr[i] = cap.filteredData(i);
    }
  }
}

// ===================== LOOP =====================
void loop() {
  unsigned long now = millis();

  updateEncoder();
  handleEncoderScroll();

  handleButton(now);
  updateJoystick(now);
  updateMPR121Sliders(now);
  updateLED(now);
}

// ===================== MPR121 =====================
void setupMPR121() {
  if (!cap.begin(0x5A)) {
    mprOK = false;
    if (DEBUG_SERIAL) Serial.println("MPR121 not found");
    return;
  }

  mprOK = true;

  // 두 번째 테스트 파일의 방식 반영: stop -> soft reset -> CDC/CDT/threshold -> run
  cap.writeRegister(0x5E, 0x00); // stop mode
  cap.writeRegister(0x80, 0x63); // soft reset
  delay(10);

  cap.writeRegister(0x5C, 0x10); // CDC: 16uA
  cap.writeRegister(0x5D, 0x20); // CDT: 0.5us
  cap.setThresholds(12, 6);
  cap.writeRegister(0x5E, 0x8F); // run mode, 12 electrodes

  if (DEBUG_SERIAL) Serial.println("MPR121 configured");
}

void updateMPR121Sliders(unsigned long now) {
  if (!mprOK) return;
  if (now - lastMprRead < MPR_INTERVAL_MS) return;
  lastMprRead = now;

  for (uint8_t i = 0; i < 12; i++) {
    uint16_t cur = cap.filteredData(i);

    // filteredData는 손이 가까워지거나 닿을수록 보통 감소한다.
    // 직전값보다 줄면 해당 전극을 이번 cycle의 입력으로 본다.
    if (prevMpr[i] > cur) {
      uint16_t drop = prevMpr[i] - cur;

      if (drop >= MPR_MIN_DROP[i]) {
        if (i < 6) hAccum[i] += drop;
        else       vAccum[i - 6] += drop;
      }
    }

    prevMpr[i] = cur;
  }

  mprCycleCount++;
  if (mprCycleCount < MPR_CYCLES_PER_DECISION) return;
  mprCycleCount = 0;

  uint16_t hTotal = totalValue(hAccum, 6);
  uint16_t vTotal = totalValue(vAccum, 6);

  float hCenter = weightedCenter(hAccum, 6);
  float vCenter = weightedCenter(vAccum, 6);

  bool hActive = hTotal >= MPR_MIN_TOTAL_DROP;
  bool vActive = vTotal >= MPR_MIN_TOTAL_DROP;

  clearAccum(hAccum, 6);
  clearAccum(vAccum, 6);

  // peak index가 아니라, 입력 분포의 중심이 어느 방향으로 이동했는지 본다.
  // 예: [0, 1, 5, 3, 0, 0]이면 중심은 2~3 사이의 연속값이 된다.
  int hDir = directionFromCenterMove(hActive, hCenter, hCenterAnchor, hInactiveWindows, REVERSE_H_SLIDER);
  int vDir = directionFromCenterMove(vActive, vCenter, vCenterAnchor, vInactiveWindows, REVERSE_V_SLIDER);

  // 줌 기능은 제거했다.
  // 두 슬라이더가 동시에 움직여도 각각 가로/세로 스크롤로만 처리한다.
  if (hDir != 0) sendHorizontalScroll(hDir * MPR_SCROLL_STEP);
  if (vDir != 0) sendVerticalScroll(vDir * MPR_SCROLL_STEP);

  debugPrintMPR(hCenter, vCenter, hTotal, vTotal, hDir, vDir);
}

int directionFromCenterMove(bool active, float center, float &anchor, uint8_t &inactiveWindows, bool reverseDir) {
  if (!active || center < 0.0) {
    if (inactiveWindows < 255) inactiveWindows++;
    if (inactiveWindows >= MPR_INACTIVE_WINDOWS_TO_RESET) {
      anchor = -1.0;
    }
    return 0;
  }

  inactiveWindows = 0;

  if (anchor < 0.0) {
    anchor = center;
    return 0;
  }

  float delta = center - anchor;

  if (delta > -MPR_MIN_CENTER_MOVE && delta < MPR_MIN_CENTER_MOVE) {
    return 0;
  }

  int dir = (delta > 0.0) ? 1 : -1;
  if (reverseDir) dir = -dir;

  // 한 번 스크롤을 보낸 뒤에는 현재 중심을 새 기준으로 잡는다.
  // 따라서 손가락이 계속 이동하면 일정 거리마다 추가 스크롤이 발생한다.
  anchor = center;
  return dir;
}

float weightedCenter(uint16_t *arr, uint8_t n) {
  uint32_t total = 0;
  uint32_t weighted = 0;

  for (uint8_t i = 0; i < n; i++) {
    total += arr[i];
    weighted += (uint32_t)arr[i] * i * 100UL;
  }

  if (total == 0) return -1.0;
  return ((float)weighted) / ((float)total * 100.0);
}

uint16_t totalValue(uint16_t *arr, uint8_t n) {
  uint32_t total = 0;
  for (uint8_t i = 0; i < n; i++) {
    total += arr[i];
  }
  if (total > 65535UL) return 65535;
  return (uint16_t)total;
}

void clearAccum(uint16_t *arr, uint8_t n) {
  for (uint8_t i = 0; i < n; i++) arr[i] = 0;
}

// ===================== ENCODER =====================
void updateEncoder() {
  int A = digitalRead(ENCODER_A);
  int B = digitalRead(ENCODER_B);
  int encoded = (A << 1) | B;
  int sum = (lastEncoded << 2) | encoded;

  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderCount++;
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderCount--;

  lastEncoded = encoded;
}

void handleEncoderScroll() {
  long delta = encoderCount - lastEncoderCountForScroll;
  if (delta == 0) return;

  if (REVERSE_ENCODER_SCROLL) delta = -delta;

  encoderScrollAccum += (int)delta;
  lastEncoderCountForScroll = encoderCount;

  while (encoderScrollAccum >= ENCODER_COUNTS_PER_SCROLL) {
    if (encoderHorizontalMode) sendHorizontalScroll(-ENCODER_SCROLL_STEP);
    else                       sendVerticalScroll(ENCODER_SCROLL_STEP);
    encoderScrollAccum -= ENCODER_COUNTS_PER_SCROLL;
  }

  while (encoderScrollAccum <= -ENCODER_COUNTS_PER_SCROLL) {
    if (encoderHorizontalMode) sendHorizontalScroll(ENCODER_SCROLL_STEP);
    else                       sendVerticalScroll(-ENCODER_SCROLL_STEP);
    encoderScrollAccum += ENCODER_COUNTS_PER_SCROLL;
  }
}

// ===================== SHARED BUTTON =====================
void handleButton(unsigned long now) {
  bool rawPressed = !digitalRead(BUTTON_PIN); // INPUT_PULLUP: LOW = pressed

  if (rawPressed != lastRawButtonPressed) {
    lastDebounceTime = now;
    lastRawButtonPressed = rawPressed;
  }

  if (now - lastDebounceTime > DEBOUNCE_MS) {
    stableButtonPressed = rawPressed;
  }

  // press edge
  if (stableButtonPressed && !lastStableButtonPressed) {
    buttonPressStart = now;
    longPressFired = false;
  }

  // optional long press reset
  if (ENABLE_LONG_PRESS_RESET && stableButtonPressed && !longPressFired && now - buttonPressStart >= LONG_PRESS_MS) {
    longPressFired = true;
    releaseArrowKeys();
    Keyboard.releaseAll();
    AdvancedMouse.end();
    Keyboard.end();
    delay(50);
    resetFunc();
  }

  // release edge
  if (!stableButtonPressed && lastStableButtonPressed) {
    unsigned long pressDuration = now - buttonPressStart;

    if (!longPressFired && pressDuration < LONG_PRESS_MS) {
      if (pendingClickCount == 0) {
        pendingClickCount = 1;
        firstClickTime = now;
      } else if (pendingClickCount == 1 && now - firstClickTime <= DOUBLE_CLICK_MS) {
        pendingClickCount = 0;
        onDoubleClick();
      }
    }
  }

  // single click is confirmed only after double-click window passes
  if (pendingClickCount == 1 && now - firstClickTime > DOUBLE_CLICK_MS) {
    pendingClickCount = 0;
    onSingleClick();
  }

  lastStableButtonPressed = stableButtonPressed;
}

void onSingleClick() {
  encoderHorizontalMode = !encoderHorizontalMode;

  if (DEBUG_SERIAL) {
    Serial.print("Encoder mode: ");
    Serial.println(encoderHorizontalMode ? "HORIZONTAL" : "VERTICAL");
  }
}

void onDoubleClick() {
  joystickMouseMode = !joystickMouseMode;
  releaseArrowKeys();
  startDoubleBlink();

  if (DEBUG_SERIAL) {
    Serial.print("Joystick mode: ");
    Serial.println(joystickMouseMode ? "MOUSE" : "ARROWS");
  }
}

// ===================== JOYSTICK =====================
void updateJoystick(unsigned long now) {
  if (now - lastJoyUpdate < JOY_INTERVAL_MS) return;
  lastJoyUpdate = now;

  int rawX = analogRead(STICK_X_PIN);
  int rawY = analogRead(STICK_Y_PIN);
  int dx = rawX - JOY_CENTER_X;
  int dy = rawY - JOY_CENTER_Y;

  if (joystickMouseMode) {
    releaseArrowKeys();

    int mx = axisToMouseStep(dx);
    int my = axisToMouseStep(dy);
    if (INVERT_MOUSE_X) mx = -mx;
    if (INVERT_MOUSE_Y) my = -my;

    if (mx != 0 || my != 0) {
      AdvancedMouse.move(mx, my, 0, 0);
    }
  } else {
    bool right = dx > JOY_DEADZONE;
    bool left  = dx < -JOY_DEADZONE;
    bool down  = dy > JOY_DEADZONE;
    bool up    = dy < -JOY_DEADZONE;

    if (INVERT_ARROW_X) {
      bool t = right; right = left; left = t;
    }
    if (INVERT_ARROW_Y) {
      bool t = down; down = up; up = t;
    }

    setKey(KEY_RIGHT_ARROW, right, rightPressed);
    setKey(KEY_LEFT_ARROW,  left,  leftPressed);
    setKey(KEY_DOWN_ARROW,  down,  downPressed);
    setKey(KEY_UP_ARROW,    up,    upPressed);
  }
}

int axisToMouseStep(int d) {
  if (abs(d) <= JOY_DEADZONE) return 0;

  int mag = abs(d) - JOY_DEADZONE;
  int step = mag / MOUSE_DIVISOR + 1;
  if (step > MOUSE_MAX_STEP) step = MOUSE_MAX_STEP;
  return (d > 0) ? step : -step;
}

void setKey(uint8_t key, bool shouldPress, bool &state) {
  if (shouldPress && !state) {
    Keyboard.press(key);
    state = true;
  } else if (!shouldPress && state) {
    Keyboard.release(key);
    state = false;
  }
}

void releaseArrowKeys() {
  if (rightPressed) { Keyboard.release(KEY_RIGHT_ARROW); rightPressed = false; }
  if (leftPressed)  { Keyboard.release(KEY_LEFT_ARROW);  leftPressed = false; }
  if (downPressed)  { Keyboard.release(KEY_DOWN_ARROW);  downPressed = false; }
  if (upPressed)    { Keyboard.release(KEY_UP_ARROW);    upPressed = false; }
}

// ===================== HID ACTIONS =====================
void sendVerticalScroll(int amount) {
  if (amount == 0) return;
  AdvancedMouse.move(0, 0, amount, 0);
}

void sendHorizontalScroll(int amount) {
  if (amount == 0) return;
  if (REVERSE_HID_HORIZONTAL_SCROLL) amount = -amount;

  // Real HID horizontal scroll: Consumer AC Pan
  AdvancedMouse.move(0, 0, 0, amount);
}

// ===================== LED =====================
void startDoubleBlink() {
  ledBlinkTransitionsLeft = 4; // ON/OFF/ON/OFF = 2 blinks
  ledBlinkOn = false;
  lastLedBlinkTime = 0;
}

void updateLED(unsigned long now) {
  if (ledBlinkTransitionsLeft > 0) {
    if (lastLedBlinkTime == 0 || now - lastLedBlinkTime >= LED_BLINK_INTERVAL_MS) {
      ledBlinkOn = !ledBlinkOn;
      analogWrite(LED_PIN, ledBlinkOn ? LED_BLINK_BRIGHTNESS : 0);
      ledBlinkTransitionsLeft--;
      lastLedBlinkTime = now;
    }
    return;
  }

  analogWrite(LED_PIN, LED_IDLE_BRIGHTNESS);
}

// ===================== DEBUG =====================
void debugPrintMPR(float hCenter, float vCenter, uint16_t hTotal, uint16_t vTotal, int hDir, int vDir) {
  if (!DEBUG_SERIAL) return;

  Serial.print("MPR hCenter="); Serial.print(hCenter, 2);
  Serial.print(" vCenter="); Serial.print(vCenter, 2);
  Serial.print(" hTotal="); Serial.print(hTotal);
  Serial.print(" vTotal="); Serial.print(vTotal);
  Serial.print(" hDir="); Serial.print(hDir);
  Serial.print(" vDir="); Serial.print(vDir);
  Serial.print(" hAnchor="); Serial.print(hCenterAnchor, 2);
  Serial.print(" vAnchor="); Serial.print(vCenterAnchor, 2);
  Serial.println(" action=SCROLL");
}
