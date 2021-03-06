void pickUp(){

            /*
             * Function which completes the whole cycle of lifting 
             * the block once it is in position, including moving
             * the lift up and down and pushing the block onto it 
             * with the flap.
             */
            Serial.println("Starting mechanism ...");
            //delay(1000);

            //liftGoingDown(myMotorLift, 255, 9000);
            //Serial.println("Lift went down");
            //delay(500);

            closeFlap(servoFlap, 40, 120);
            Serial.println("Flap closed");
            delay(500);
            
            liftGoingUp(myMotorLift, 255, 10000);
            Serial.println("Lift went up");
            delay(500);

}
