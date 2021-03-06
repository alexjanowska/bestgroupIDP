#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include<Servo.h> 

Servo servoFlap;
Servo servoArm;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();

Adafruit_DCMotor *myMotorLeft = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorRight = AFMS.getMotor(2);
Adafruit_DCMotor *myMotorLift = AFMS.getMotor(3);


// function and variable definitions
// bool moveToWall(int distance_limit, int distance_no_speed);
const int trigPinFront = 3; // Trigger Pin of Ultrasonic Sensor
const int echoPinFront = 2; // Echo Pin of Ultrasonic Sensor
const int trigPinSide = 6;
const int echoPinSide = 7;
const int flashLED = 12;   // pin number for the flashing led  // TODO! 
const int photoPin = 8; // Phototransistor Pin - high for block, low for no block
const int hallPin = 5;  // hall effect sensor pin
const int microPin = 11;

long duration = 0;
float distance = 0;
float motor_speed = 0;
int distance_limit = 30;
int distance_no_speed = 20;
bool atWall, hall;
long olddist;
int stage;
int nextTurn = 1; // 1 if next turn is right 90, 2 if next turn is right 180, 3 if next turn is left 180
int sweep = 0;
int autoCounter = 0, liftCounter = 0;
float sideDist, sideDistOld, diff;
float kp = 20;
bool rwheel;
int pos;
int var;
bool isThereABlock;

int time_a = 0;
int time_b = 0;
int time_gap = 2000;
int iterations = 0;

int flashLEDstate = LOW;
unsigned long previousMillis = 0;
const long interval = 500;


void setup() {
  Serial.begin(9600);
  Serial.println("Hello! I'm Henry the Robot.");

  // initialise right and left motors, set initial speed and readings
  myMotorLeft->run(RELEASE);
  myMotorRight->run(RELEASE);
  myMotorLift->run(RELEASE);

  delay(3000);

  // ultrasound pin setup - trigger output, echo input
  pinMode(trigPinFront, OUTPUT); 
  pinMode(echoPinFront, INPUT);
  pinMode(trigPinSide, OUTPUT); 
  pinMode(echoPinSide, INPUT);
  pinMode(microPin, INPUT);

  pinMode(photoPin, INPUT);
  pinMode(hallPin, INPUT);
  pinMode(flashLED, OUTPUT);

  // initialise motor operation
  AFMS.begin();  // create with the default frequency 1.6KHz

  // initialise right and left motors, set initial speed and readings
  myMotorLeft->run(RELEASE);
  myMotorRight->run(RELEASE);
  myMotorLift->run(RELEASE);
  
  motor_speed = 255;
  atWall = false;
  sideDist = get_distance(2);
  
  // setting up the servo motors
  servoFlap.attach(10); // the pin!
  servoArm.attach(9); // the pin! 

  stage = 0;
  var = 0;

  //------ a few initial mechanical set ups made--------------
  bringArmToNeutral(servoArm, 0);
  Serial.println("Servo arm has been brought to neutral");
  delay(50);
  
  digitalWrite(flashLED, flashLEDstate);

  closeFlap(servoFlap, 40, 120);
  Serial.println("Flap set to neutral CLOSED position");
  delay(5000);

  liftGoingUp(myMotorLift, 255, 7000);
  Serial.println("Lift is now at the top to start off with!");
  //----------------------------------------------------------
  
}


void loop() {

  // -------This bit of code is to keep the flash led flashing--
  // -------every 1 second everytime loop is running------------
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    if(flashLEDstate == LOW){
      flashLEDstate = HIGH;
    }
    else flashLEDstate = LOW;
  }
  digitalWrite(flashLED, flashLEDstate);
  // -----------------------------------------------------------


  // counter for path auto-correction - checks every 20 loops 
  autoCounter += 1;
  if(autoCounter > 20) {
    autoCounter = 0;
  }

  // compares previous and current distance from wall, adjusts path proportionally to difference
  if(autoCounter == 0){
    sideDistOld = sideDist;
    sideDist = get_distance(2);
    diff = sideDist - sideDistOld;
    if(sideDistOld > sideDist){
      rwheel = true;
      diff = -diff;
    }
    else if(sideDistOld < sideDist){
      rwheel = false;
    }
    else{
      diff = 0;
    }
    
    // differences above 7cm discarded as anomalous
    // differences while turning set to 0
    if(diff > 5.0 || stage != 0) diff = 0;
    
  }

 
  
  distance = get_distance(1);
  //Serial.println(distance);

  time_b = millis();
  time_a = millis();
  
  switch(stage){
    case 0: // moves FORWARD till WALL

          // drop off shelf causes some error in ultrasound behaviour - readings disregarded when US pointing at shelf
          if(distance > 93 && distance < 140 && nextTurn == 3 && sweep != 1 && sweep < 6) diff = 0; 
          //if(sweep > 6) diff = 0; 
          if(sweep == 6){ // if in the last portion of journey, go halfway and turn towards shelf
            distance_limit = 130;
            distance_no_speed = 120;
            nextTurn = 4;
          }
          else if(sweep == 7) nextTurn = 5; // if facing to shelf, go forward and stop completely 
          else if(sweep == 8){ 
            nextTurn = 7;
            distance_limit = 35;
            distance_no_speed = 25;
          }
          
          isThereABlock = checkForBlock();
          //isThereABlock = false;                  // comment out for actual testing!
          if(isThereABlock==true){
            Serial.println("BLOCK DETECTED!");
            //delay(1000);
            
            stopRLMotors(5000, myMotorRight, myMotorLeft);
            Serial.println("RL Motors stopped!");
            closeFlap(servoFlap, 40, 120);
            Serial.println("Servo flap closed!");


            Serial.println("Taking lift up");
            liftGoingUp(myMotorLift, 255, 7000);

            Serial.println("Taking lift down");
            liftGoingDown(myMotorLift, 255, 9000);
            openFlap(servoFlap, 120, 40);
            Serial.println("Servo flap has been opened!");
            

            motor_speed = 40;
            iterations = 0;
            Serial.println("Entering while loop");
            while(iterations < 15){
              //moveForward(myMotorRight, motor_speed, myMotorLeft, motor_speed, 100);
              moveForward(myMotorLeft, motor_speed, myMotorRight, motor_speed, 50);
              iterations+=1;
              Serial.print(iterations);
              if(isHallActive()==true){
                Serial.println("MAGNETIC BLOCK DETECTED");
                delay(1000);
                liftGoingUp(myMotorLift, 255, 10000);
                stage = 0;
                break;
              }
            }
            
            stopRLMotors(2000, myMotorRight, myMotorLeft);
            // by this point, block would be ready to be pushed on to the lift
            // code will go to stage 9 if the hall sensor doesn't become active within the next time_gap milliseconds
            stage = 9;  // for the pick up
            break;
          }


          // move forward with course correction until wall reached
          atWall = moveToWall(distance_limit, distance_no_speed);

          // when wall reached, move onto next stage 
          if(atWall == true){ // turns when gets close to wall
            stage = nextTurn;
          }

          break;
          
    case 1: 
          // turn RIGHT 90 degrees
         
          Serial.println("turning 90 right");
          //Serial.println("you made it into case 1 well done");
          motor_speed = 100;
          turnRight(myMotorLeft, motor_speed, myMotorRight, 0, 4800);
          stage = 0;
          nextTurn += 1;
          sweep = 1;
          distance_limit = 35;
          distance_no_speed = 25;
          break;

    case 2: 
          // turn RIGHT 180 degrees
          Serial.println("Inside stage 2!");
          
          Serial.println("turning 180 right");
          motor_speed = 100;
          turnRight(myMotorLeft, motor_speed, myMotorRight, 0, 9600);
          stage = 0;
          nextTurn += 1;
          sweep += 1;
          break;
          
    case 3: 
          // turn LEFT 180 degrees
          Serial.println("turning 180 left");
          motor_speed = 100;
          turnLeft(myMotorLeft, 0, myMotorRight, motor_speed, 9600);
          stage = 0;
          nextTurn -= 1;
          sweep += 1;
          break;
          
    case 4: 
          // turn LEFT 90 degrees
          
          Serial.println("turning 90 left");
          motor_speed = 100;
          turnLeft(myMotorLeft, 0, myMotorRight, motor_speed, 4800);
          stage = 0;
          sweep = 7;
          distance_limit = 20;
          distance_no_speed = 10;
          break;

    case 5: 
          // at loading place
          stopRLMotors(5000, myMotorRight, myMotorLeft);
          stage = 6;
          break;
          
    case 6: // parking - leaving loading shelf and turning towards finish box
          motor_speed = 255;
          moveBackwards(myMotorLeft, motor_speed, myMotorRight, motor_speed, 1000);
          motor_speed = 100;
          turnLeft(myMotorLeft, 0, myMotorRight, motor_speed, 4800);
          stage = 0;
          sweep = 8;
          break;
          
    case 7: // parking - combined with case 8
          turnRight(myMotorLeft, motor_speed, myMotorRight, 0, 4800);
          stage = 8;
          break;
          
    case 8: // parking - continuation from case 7
          stopRLMotors(5000, myMotorRight, myMotorLeft);
          break;

    case 9:
            // After detecting a block and magnetic sensor doesn't become active
            motor_speed = 30;
            moveForward(myMotorLeft, motor_speed, myMotorRight, motor_speed, 1000);
            stopRLMotors(2000, myMotorRight, myMotorLeft);
            pickUp();
            stage = 0;
            break;
          
     case 10:  // TESTING THE SERVO FLAP - OPEN AND CLOSE
            Serial.println("Inside case 13!");
            openFlap(servoFlap, 120, 40);
            Serial.println("FLAP OPENED");
            delay(3000);
            closeFlap(servoFlap, 40, 120);
            Serial.println("FLAP CLOSED");
            delay(3000);
            break;


     case 11: // TO TEST THE HALL SENSOR
            hall = isHallActive();
            if(hall==true){
              Serial.println("READING HIGH");
              delay(300);
              //var = 1;
            }
            else if(hall==false){
              Serial.println("READING LOW");
              delay(300);
              //var = 0;
            }
            break;

     case 12:   // TO TEST THE PHOTO SENSOR
            Serial.println("in case 21");
            if(isPhotoActive()==true){
              Serial.println("READING HIGH");
              delay(100);
             
            }
            else if(isPhotoActive()==false){
              Serial.println("READING LOW");
              delay(100);
           
            }
            break;

     case 13:   // TO TEST THE MICROSWITCH
            Serial.println("Inside case 22");
            delay(300);
            if(isMicroswitchPressed(microPin)==true){
              Serial.println("READING HIGH");
              delay(100);
              //var = 1;
            }
            else if(isMicroswitchPressed(microPin)==false){
              Serial.println("READING LOW");
              delay(100);
              //var = 0;
            }
            break;

  }
}
