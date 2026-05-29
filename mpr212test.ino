#include <Wire.h>
#include "Adafruit_MPR121.h"

Adafruit_MPR121 cap = Adafruit_MPR121();

void setup() {
  Serial.begin(115200);
  while (!Serial); 

  // MPR121 시작 (기본 주소 0x5A)
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found!");
    while (1);
  }

  // 레지스터 설정을 위한 Stop 모드
  cap.writeRegister(0x5E, 0x00); 

  // [중요] 과전압으로 꼬였을 수 있는 내부 상태 초기화 (소프트 리셋)
  cap.writeRegister(0x80, 0x63); 
  delay(10);

  // 전극 충전 전류(CDC) 설정 - 표준값 0x10 (16uA)
  cap.writeRegister(0x5C, 0x10); 
  // 충전 시간(CDT) 설정 - 0x20 (0.5us)
  cap.writeRegister(0x5D, 0x20); 

  // 임계값 설정 (정상 전압에서는 12, 6 정도가 적당)
  for (uint8_t i=0; i<12; i++) {
    cap.setThresholds(12, 6);
  }

  // 다시 실행 모드 (12개 전극 활성화)
  cap.writeRegister(0x5E, 0x8F); 

  Serial.println("MPR121 Re-Configured on VIN.");
}

void loop() {
  // RAW 데이터 벡터 출력
  Serial.print("MPR_RAW:[");
  for (uint8_t i = 0; i < 12; i++) {
    Serial.print(cap.filteredData(i));
    if (i < 11) Serial.print(",");
  }
  Serial.println("]");
  
  delay(100);
}