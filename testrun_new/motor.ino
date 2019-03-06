void moveForward(Adafruit_DCMotor *motor1, long motor_speed_1, Adafruit_DCMotor *motor2, long motor_speed_2, long dur){

  motor1->setSpeed(motor_speed_1);
  motor2->setSpeed(motor_speed_2);

  motor1->run(BACKWARD);
  motor2->run(FORWARD);

  delay(dur);
}

void moveBackwards(Adafruit_DCMotor *motor1, long motor_speed_1, Adafruit_DCMotor *motor2, long motor_speed_2, long dur){

  motor1->setSpeed(motor_speed_1);
  motor2->setSpeed(motor_speed_2);

  motor1->run(FORWARD);
  motor2->run(BACKWARD);

  delay(dur);

  for(int i = 100; i >= 0; i--){
    motor1->setSpeed(i);
    motor2->setSpeed(i);
    delay(10);
  }
}

void turnRight(Adafruit_DCMotor *motor1, long motor_speed_1, Adafruit_DCMotor *motor2, long motor_speed_2, long dur){

  motor1->setSpeed(motor_speed_1);
  motor2->setSpeed(motor_speed_2);

  motor1->run(BACKWARD);
  motor2->run(BACKWARD);

  delay(dur);

  motor1->run(RELEASE);
  motor2->run(RELEASE);


  
}

void turnLeft(Adafruit_DCMotor *motor1, long motor_speed_1, Adafruit_DCMotor *motor2, long motor_speed_2, long dur){

  motor1->setSpeed(motor_speed_1);
  motor2->setSpeed(motor_speed_2);

  motor2->run(FORWARD);

  delay(dur);

  motor1->run(RELEASE);
  motor2->run(RELEASE);
}

void stopMotor(Adafruit_DCMotor *motor1, Adafruit_DCMotor *motor2, long dur){
  motor1->setSpeed(0);
  motor2->setSpeed(0);

  motor1->run(RELEASE);
  motor2->run(RELEASE);

  delay(dur);


bool isStuck(){
  
}

void closeFlap(Servo &servoFlap){
  servoFlap.write(0);
  delay(2000);
=======
/*
void closeFlap(Servo servoFlap){
  servoFlap.write(90);
  delay(500);
>>>>>>> 7924d51e52c72014a517d5b5864fff210ea766a2
}

void openFlap(Servo &servoFlap){
  servoFlap.write(180);
  delay(2000);
  
}

void sweepTheArm(Servo servoArm){
  // function that sweeps the unloading arm and then brings it back to neutral position
  servoArm.write(90);
  delay(2000);
  servoArm.write(0);
  delay(2000);
}
<<<<<<< HEAD


void liftGoingUp(){
  
}

void liftGoingDown() {
  
}
=======
*/
