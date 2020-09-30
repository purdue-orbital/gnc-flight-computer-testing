//Super Simple piece of code to test out the LSM6DS3's accelerometers with arduino

#include <SparkFunLSM6DS3.h>
#include <Wire.h> 

#define kLocalG -9.8
#define kLedPin 13

LSM6DS3 imu(I2C_MODE, 0x6a);

//Program runs a quick "calibration" routine on startup and just uses these 
//offsets to zero out the accelerometers assuming it starts facing with the z
//axis facing upwards:
double xCalibOffset = 0; 
double yCalibOffset = 0;
double zCalibOffset = 0;

double accels[] = {0,0,0}; //acceleration in all three axis
double normAccels[] = {0,0,0}; //normalized acceleration in all three axis

void setup() {
  Serial.begin(9600); //Start serial communication w/ computer over usb
  pinMode(kLedPin, OUTPUT);

  initSensor(); //Do the zero routine
}

void loop() {
  updateAccels(); //updates accel and normaccel values

  //Prints out roll and pitch data over serial in an easily parsible format
  Serial.print(getRoll());
  Serial.print("|");
  Serial.print(getPitch());
  Serial.print("|");
  Serial.print("\n");
}

void initSensor(){
  if (imu.begin() != 0){
    //Start the imu, if it doesnt return 0 something wrong
    Serial.println("Initialization Failed");
  }else{
    Serial.println("Initialization Complete");
  }

  calibrate(10, 100); //run the zero routine
}

void calibrate(int samplePoints, double sampleDelay){
  setSignalLight(true); //Just turn on the led so you know not to move the board around
  
  double xSum = 0;
  double ySum = 0;
  double zSum = 0;
  //accumulate the accelerometer offsets over a bunch of samples to eliminate the effect of noise
  for(int i = 0; i<samplePoints; i++){
    xSum += getRawAccelX();
    ySum += getRawAccelY();
    zSum += getRawAccelZ() - kLocalG; //subtract g because we expect this value to be -9.8 on startup (board is facing up)

    delay(sampleDelay); //wait a tiny bit before collecting the next sample
  }

  //set the calibration offsets to the samplepoints
  xCalibOffset = xSum / samplePoints;
  yCalibOffset = ySum / samplePoints;
  zCalibOffset = zSum / samplePoints;
  
  Serial.println(xCalibOffset);
  Serial.println(yCalibOffset);
  Serial.println(zCalibOffset);
  setSignalLight(false); //turn off the led
}

void setSignalLight(boolean on){
  if (on){
    digitalWrite(kLedPin, HIGH);
  }else{
    digitalWrite(kLedPin, LOW);
  }
}

void updateAccels(){
  //subtract the offsets from the raw data
  accels[0] = getRawAccelX() - xCalibOffset;
  accels[1] = getRawAccelY() - yCalibOffset;
  accels[2] = getRawAccelZ() - zCalibOffset;

  double mag = sqrt(
    accels[0] * accels[0] +
    accels[1] * accels[1] +
    accels[2] * accels[2]
  );
  
  normAccels[0] = accels[0] / mag;
  normAccels[1] = accels[1] / mag;
  normAccels[2] = accels[2] / mag;
}

double getPitch(){
  //arc sin of the -x axis / kLocalG converted to degrees
  return asin(-accels[0] / kLocalG) * 180/PI;
}

double getRoll(){
  //arc sin of the -x axis / kLocalG converted to degrees
  return atan(accels[1] / accels[2]) * 180/PI;
}

double getRawAccelX(){
  //use the library to get the raw accelerations from the board
  return imu.readFloatAccelX() * kLocalG;
}

double getRawAccelY(){
  //use the library to get the raw accelerations from the board
  return imu.readFloatAccelY() * kLocalG;
}

double getRawAccelZ(){
  //use the library to get the raw accelerations from the board
  return imu.readFloatAccelZ() * kLocalG;
}
