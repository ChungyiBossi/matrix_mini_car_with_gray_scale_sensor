#include <MiniBTN.h>
#include <MiniRC.h>
#include <MiniLED.h>
#include <MiniDIG.h>
#include <MiniDC.h>
#include <MiniANG.h>
#include <MatrixMini.h>

int defalutDelayTime = 10;
int defaultRotationDelayTime = 50;
int defaultCarWheelSpeed = 30;
int preGrayScaleSensorValue;

int stableForwardCount = 0; // 用來確定前進是否穩定
int rotationSide = 1; // 1:順, -1:逆
int rotateCount = 0; //當前的旋轉次數
void setup() {
  // put your setup code here, to run once:
  Mini.begin(7.4, 1, 115200);
  preGrayScaleSensorValue = Mini.A3.getANG();
}

void loop() {
  // front ultrasonic sensor
  int distanceInCM = Mini.D1.US.get();
  Serial.print("Distance in cm:");
  Serial.println(distanceInCM);
  // grayScaleSensor
  int grayScaleSensorValue = Mini.A3.getANG();
  bool from_isBlack = isScaleBlack(preGrayScaleSensorValue);
  bool to_isBlack = isScaleBlack(grayScaleSensorValue);
  float ratio = 1;
  if (!to_isBlack) {
    RoadMiss();
  } else if (distanceInCM < 5){
    MeetWall();
  } else {
    Foward();
  }

  Serial.print(preGrayScaleSensorValue);
  Serial.print(" -> ");
  Serial.print(grayScaleSensorValue);
  Serial.print(", ratio: ");
  Serial.println(ratio);
  preGrayScaleSensorValue = grayScaleSensorValue;
}

bool isScaleBlack(int sensorValue){
  if (sensorValue > 600){ return true;}
  else { return false;}
}

void MeetWall(){
  // wall in front of the car
  Serial.println("Wall");
  Mini.RGB1.setRGB(0, 0, 255); // BLUE
  BackAndFind();
}

void RoadMiss(){
  // Any->白: 先往後再旋轉: RGB=RED
  Serial.println("Any->W");    
  Mini.RGB1.setRGB(255, 0, 0); // RED
  BackAndFind();
}

void BackAndFind(){
  delay(100);
  if (rotateCount % 2 == 0){
    rotationSide = 1;
  } else {
    rotationSide = -1;
  }
  setSpeed(-1, -1, 1.1 * defalutDelayTime);
  setSpeed(1 * rotationSide, -1 * rotationSide, defaultRotationDelayTime * (rotateCount + 1));
  rotateCount++;
  stableForwardCount = 0;
}

void Foward(){
  // Any->黑：前進: RGB=WHITE
  Serial.println("Foward");
  Mini.RGB1.setRGB(255, 255, 255);  // White
  setSpeed(1, 1, defalutDelayTime);
  stableForwardCount += 1;
  if (stableForwardCount > 10){
    // TODO: 強化穩定前進的判斷
    rotateCount = 0;
  }
}

void setSpeed(float rightSpeedScale, float leftSpeedScale, float delayTime){
  Mini.M1.set((int)(rightSpeedScale * defaultCarWheelSpeed));
  Mini.M2.set((int)(leftSpeedScale * defaultCarWheelSpeed));
  delay(int(delayTime));
  Mini.M1.set(0);
  Mini.M2.set(0);
}
