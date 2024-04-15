#include <Wire.h>
#include "PS2X_lib.h"
#include "QGPMaker_MotorShield.h"
#include "QGPMaker_Encoder.h"

QGPMaker_MotorShield AFMS = QGPMaker_MotorShield();
PS2X ps2x;
long ARM_MIN[]={10, 10, 40, 10};

long ARM_MAX[]={170, 140, 170, 102};

QGPMaker_Servo *Servo1 = AFMS.getServo(0);
QGPMaker_Servo *Servo2 = AFMS.getServo(1);
QGPMaker_Servo *Servo3 = AFMS.getServo(2);
QGPMaker_Servo *Servo4 = AFMS.getServo(3);
QGPMaker_DCMotor *DCMotor_2 = AFMS.getMotor(2);
QGPMaker_DCMotor *DCMotor_4 = AFMS.getMotor(4);
QGPMaker_DCMotor *DCMotor_1 = AFMS.getMotor(1);
QGPMaker_DCMotor *DCMotor_3 = AFMS.getMotor(3);


QGPMaker_Encoder Encoder1(1);
QGPMaker_Encoder Encoder2(2);
QGPMaker_Encoder Encoder3(3);
QGPMaker_Encoder Encoder4(4);

 
volatile double pos1;
volatile double pos2;
volatile double pos3;
volatile double pos4;
volatile double rpm1;
volatile double rpm2;
volatile double rpm3;
volatile double rpm4;
volatile int PPR;
volatile int gearratio;
volatile int CPR;

void forward() {
  DCMotor_1->setSpeed(200);
  DCMotor_1->run(FORWARD);
  DCMotor_2->setSpeed(200);
  DCMotor_2->run(FORWARD);
  DCMotor_3->setSpeed(200);
  DCMotor_3->run(FORWARD);
  DCMotor_4->setSpeed(200);
  DCMotor_4->run(FORWARD);
}

void turnLeft() {
  DCMotor_1->setSpeed(200);
  DCMotor_1->run(BACKWARD);
  DCMotor_2->setSpeed(200);
  DCMotor_2->run(FORWARD);
  DCMotor_3->setSpeed(200);
  DCMotor_3->run(BACKWARD);
  DCMotor_4->setSpeed(200);
  DCMotor_4->run(FORWARD);
}

void turnRight() {
  DCMotor_1->setSpeed(200);
  DCMotor_1->run(FORWARD);
  DCMotor_2->setSpeed(200);
  DCMotor_2->run(BACKWARD);
  DCMotor_3->setSpeed(200);
  DCMotor_3->run(FORWARD);
  DCMotor_4->setSpeed(200);
  DCMotor_4->run(BACKWARD);
}

void moveLeft() {
  DCMotor_1->setSpeed(200);
  DCMotor_1->run(BACKWARD);
  DCMotor_2->setSpeed(200);
  DCMotor_2->run(FORWARD);
  DCMotor_3->setSpeed(200);
  DCMotor_3->run(FORWARD);
  DCMotor_4->setSpeed(200);
  DCMotor_4->run(BACKWARD);
}

void moveRight() {
  DCMotor_1->setSpeed(200);
  DCMotor_1->run(FORWARD);
  DCMotor_2->setSpeed(200);
  DCMotor_2->run(BACKWARD);
  DCMotor_3->setSpeed(200);
  DCMotor_3->run(BACKWARD);
  DCMotor_4->setSpeed(200);
  DCMotor_4->run(FORWARD);
}

void backward() {
  DCMotor_1->setSpeed(200);
  DCMotor_1->run(BACKWARD);
  DCMotor_2->setSpeed(200);
  DCMotor_2->run(BACKWARD);
  DCMotor_3->setSpeed(200);
  DCMotor_3->run(BACKWARD);
  DCMotor_4->setSpeed(200);
  DCMotor_4->run(BACKWARD);
}

void stopMoving() {
  DCMotor_1->setSpeed(0);
  DCMotor_1->run(RELEASE);
  DCMotor_2->setSpeed(0);
  DCMotor_2->run(RELEASE);
  DCMotor_3->setSpeed(0);
  DCMotor_3->run(RELEASE);
  DCMotor_4->setSpeed(0);
  DCMotor_4->run(RELEASE);
}


void startMove(float vec[]){
  /*
  float pwm_ratio = 80.; // pwm / velocity(m/s)
  DCMotor_1->setSpeed(abs(vec[0])*pwm_ratio);
  DCMotor_2->setSpeed(abs(vec[1])*pwm_ratio);
  DCMotor_3->setSpeed(abs(vec[2])*pwm_ratio);
  DCMotor_4->setSpeed(abs(vec[3])*pwm_ratio);
*/
  if (vec[0] > 0){
    DCMotor_1->run(FORWARD);
    DCMotor_4->run(FORWARD);    
  }
  else if(vec[0] < 0){
    DCMotor_1->run(BACKWARD);
    DCMotor_4->run(BACKWARD);  
  }
  else{
    DCMotor_1->run(BRAKE);
    DCMotor_4->run(BRAKE);   
  }
  

  if (vec[1] > 0){
    DCMotor_2->run(FORWARD);
    DCMotor_3->run(FORWARD); 
  }
  else if(vec[1] < 0){
    DCMotor_2->run(BACKWARD);
    DCMotor_3->run(BACKWARD);  
  }
  else{
    DCMotor_2->run(BRAKE);
    DCMotor_3->run(BRAKE);   
  }
  
  
}
// 차량의 중심과 오른쪽 앞 바퀴의 vx = x , vy = b
float ra = 3;
float rb = 4;

float *set_velocity(float y, float x, float w){
  // max x, y == 1 m/s
  float static v[4];

/*
  v[0] = -x + y;
  v[1] = x + y;
  v[2] = x + y;
  v[3] = -x + y;
*/
  v[0] = x + y;
  v[1] = -x + y;
  v[2] = -x + y;
  v[3] = x + y;

  //단위 m/s
  return v;
  }


 

void setup(){  
  AFMS.begin(50);
  Serial.begin(9600);
  int error = 0;
  do{
    error = ps2x.config_gamepad(13,11,10,12, true, true);
    if(error == 0){      
      Serial.println("Found Controller, configured successful");      
     }      
    else if(error == 1)      
      Serial.println("No controller found, check wiring or reset the Arduino");      
    else if(error == 2)      
      Serial.println("Controller found but not accepting commands");         
    else if(error == 3)      
      Serial.println("Controller refusing to enter Pressures mode, may not support it.");     
    if(error == 0){
      break;
    }else{
      delay(100);
    }
  }while(1);
  pos1 = 0;
  pos2 = 0;
  pos3 = 0;
  pos4 = 0;
  
  rpm1 = 0;
  rpm2 = 0;
  rpm3 = 0;
  rpm4 = 0;
  
  PPR = 12;
  gearratio = 30;
  CPR = (PPR*4) * gearratio;
  
  Servo1->writeServo(90);
  Servo2->writeServo(90);
  Servo3->writeServo(90);
  Servo4->writeServo(60);
 
}
double before = millis();
float theta_change_before = millis();
float interval = 0;
float vx;
float vy;
float w = 0;
//float velocity[4];
float theta = -PI/6;
float theta_interval = PI/2;
int add_sw = 0;
float kp = 1;
float ki = 0.1;
float kd = 0.1;

float Target_RPM1 = 0;
float Target_RPM2 = 0;
float Target_RPM3 = 0;
float Target_RPM4 = 0;

float rpm_error1 = 0;
float rpm_error2 = 0;
float rpm_error3 = 0;
float rpm_error4 = 0;

float rpm_error1_before = 0;
float rpm_error2_before = 0;
float rpm_error3_before = 0;
float rpm_error4_before = 0;

float eintegral1 = 0.;
float eintegral2 = 0.;
float eintegral3 = 0.;
float eintegral4 = 0.;

float dedt1 = 0.;
float dedt2 = 0.;
float dedt3 = 0.;
float dedt4 = 0.;

float control1 = 0.;
float control2 = 0.;
float control3 = 0.;
float control4 = 0.;

int now_pwm1 = 0;
int now_pwm2 = 0;
int now_pwm3 = 0;
int now_pwm4 = 0;

int Change_vector_interval = 1000; // ms
float rpm_per_1m_s = 238.7;

float clip_overload(float a){
  if (a > 10000){
    a = 10000;
    return a;
  }
  else if (a < -10000){
    a = -10000;
    return a;
  }
  else return a;
}
void loop(){
    // 엔코더 출력 부분
   
    
    
    // 실행 부분
    
   if (millis() - theta_change_before > Change_vector_interval){
    // 시간에 따라 벡터 방향 변경
      theta_change_before = millis();
      // CCW -> CW -> CCW -> CW repeat
      /*
      //theta += theta_interval;
      if (add_sw == 0){
          theta += theta_interval;
          if (theta >= 2*PI-theta_interval*0.5){
            add_sw = 1;
          }
      }
      else{
        theta -= theta_interval;
        if (theta <= 0){
            add_sw = 0;
        }
      }*/
      /*
      // Draw circle
      theta += theta_interval;
      if(theta >= 2*PI-theta_interval*0.5){
        theta = 0;
      }
      */
      theta = PI/2;
      
   }

   
    vx = 0.5*cos(theta);
    vy = 0.5*sin(theta);
    
    float* velocity = set_velocity(vy, vx, w);
    /*
    Serial.print("theta = ");
    Serial.println(theta);
    Serial.print("vx,vy = ");
    Serial.print(vx);
    Serial.print(", ");
    Serial.println(vy);
    Serial.print("velocity = ");
    Serial.print(velocity[0]);
    Serial.print(", ");
    Serial.print(velocity[1]);
    Serial.print(", ");
    Serial.print(velocity[2]);
    Serial.print(", ");
    Serial.println(velocity[3]);
    Serial.println();
*/
    
    interval = millis() - before;
    
    
    pos1 = Encoder1.read();
    pos2 = Encoder2.read();
    pos3 = Encoder3.read();
    pos4 = Encoder4.read();
    
    rpm1 = -(60000/interval) * (pos1/CPR);
    rpm2 = (60000/interval) * (pos2/CPR);
    rpm3 = -(60000/interval) * (pos3/CPR);
    rpm4 = (60000/interval) * (pos4/CPR);
    
    Encoder1.write(0);
    Encoder2.write(0);
    Encoder3.write(0);
    Encoder4.write(0);
    
    before = millis();

    // TODO back 의 경우 고려해줘야함
    // Target RPM = velocity[0] * 238.7
    // 
    Target_RPM1 = velocity[0] * rpm_per_1m_s; 
    Target_RPM2 = velocity[1] * rpm_per_1m_s;
    Target_RPM3 = velocity[2] * rpm_per_1m_s;
    Target_RPM4 = velocity[3] * rpm_per_1m_s;

    //P control
    rpm_error1 = Target_RPM1 - rpm1;
    rpm_error2 = Target_RPM2 - rpm2;
    rpm_error3 = Target_RPM3 - rpm3;
    rpm_error4 = Target_RPM4 - rpm4;


    //D control
    dedt1 = (rpm_error1-rpm_error1_before)/(interval*0.001);
    rpm_error1_before = rpm_error1;
    dedt2 = (rpm_error2-rpm_error2_before)/(interval*0.001);
    rpm_error2_before = rpm_error2;
    dedt3 = (rpm_error3-rpm_error3_before)/(interval*0.001);
    rpm_error3_before = rpm_error3;
    dedt4 = (rpm_error4-rpm_error4_before)/(interval*0.001);
    rpm_error4_before = rpm_error4;
    
    //I control
    eintegral1 = eintegral1 + rpm_error1*(interval*0.001);
    eintegral2 = eintegral2 + rpm_error2*(interval*0.001);
    eintegral3 = eintegral3 + rpm_error3*(interval*0.001);
    eintegral4 = eintegral4 + rpm_error4*(interval*0.001);
    // control
    control1 = kp*rpm_error1 + kd*dedt1 + ki*eintegral1;
    control2 = kp*rpm_error2 + kd*dedt2 + ki*eintegral2;
    control3 = kp*rpm_error3 + kd*dedt3 + ki*eintegral3;
    control4 = kp*rpm_error4 + kd*dedt4 + ki*eintegral4;
    //
    float pwm_ratio = 80.; // pwm / velocity(m/s)
    now_pwm1 += control1 * 50 / Target_RPM1;
    now_pwm2 += control2 * 50 / Target_RPM2;
    now_pwm3 += control3 * 50 / Target_RPM3;
    now_pwm4 += control4 * 50 / Target_RPM4;

    if (now_pwm1 > 255){
    now_pwm1 = 255;
    }
    else if (now_pwm1 < 0){
      now_pwm1 = 0;
    }
    if (now_pwm2 > 255){
    now_pwm2 = 255;
    }
    else if (now_pwm2 < 0){
      now_pwm2 = 0;
    }
    if (now_pwm3 > 255){
    now_pwm3 = 255;
    }
    else if (now_pwm3 < 0){
      now_pwm3 = 0;
    }
    if (now_pwm4 > 255){
    now_pwm4 = 255;
    }
    else if (now_pwm4 < 0){
      now_pwm4 = 0;
    }

    
    DCMotor_1->setSpeed(now_pwm1);
    DCMotor_2->setSpeed(now_pwm2);
    DCMotor_3->setSpeed(now_pwm3);
    DCMotor_4->setSpeed(now_pwm4);

    startMove(velocity);
    //Serial.print("interval = ");
    //Serial.println(interval);

    Serial.print(" now_pwm1 = ");
    Serial.print(now_pwm1);
    Serial.print(" now_pwm2 = ");
    Serial.print(now_pwm2);
    Serial.print(" now_pwm3 = ");
    Serial.print(now_pwm3);
    Serial.print(" now_pwm4 = ");
    Serial.println(now_pwm4);
    
    /*
    Serial.print(" error1 = ");
    Serial.print(rpm_error1);
     Serial.print(" error2 = ");
    Serial.print(rpm_error2);
     Serial.print(" error3 = ");
    Serial.print(rpm_error3);
     Serial.print(" error4 = ");
    Serial.println(rpm_error4);
    */
    /*
    Serial.print(" rpm1 = ");
    Serial.print(rpm1);
    Serial.print(" rpm2 = ");
    Serial.print(rpm2);
    Serial.print(" rpm3 = ");
    Serial.print(rpm3);
    Serial.print(" rpm4 = ");
    Serial.println(rpm4);
    */
    
    
    
  }
