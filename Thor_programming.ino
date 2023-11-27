//All these motors are connected to a bipolar stepper motor driver A4988. We use only the STEP and DIR pins (control pins), 12V motors
//All inputs work with 5V

const int Opto_Art1 =42;
const int Opto_Art2 =44;
const int Opto_Art3 =48;
const int Opto_Art4 =49;
const int SW_Art56 =47;

//OUTPUTS PIN DEFINITION
const int Art1_STEP =28;
const int Art1_DIR =36;

const int Art2_Left_STEP =26;
const int Art2_Left_DIR =34;

const int Art2_Right_STEP =24;
const int Art2_Right_DIR =32;

const int Art3_Belt_STEP =22;
const int Art3_Belt_DIR =30;

const int Art4_STEP =23;
const int Art4_DIR =31;

const int Art56_Left_STEP =25;
const int Art56_Left_DIR =33;

const int Art56_Right_STEP =27;
const int Art56_Right_DIR =35;

const int ClawPWM =7;

void setup() {
//Code that runs once
 Serial.begin(9600);
while(!Serial);
//INPUTS PIN DEFINITION


pinMode(LED_BUILTIN, OUTPUT);

//SENSORS INPUTS
pinMode(42,INPUT); //Art1 Optodisk 5V
pinMode(44,INPUT); //Art2 Optodisk 5V
pinMode(48,INPUT); //Art3 LOWER Optodisk 5V
pinMode(49,INPUT); //Art4 UPPER Optodisk 5V
pinMode(47,INPUT); //Art56 Endstop Switch 5V

//STEP OUTPUTS-------------------------------------
pinMode(28,OUTPUT); //ART 1 STEP BASE
pinMode(26,OUTPUT); //ART 2 STEP LEFT MOTOR
pinMode(24,OUTPUT); //ART 2 STEP RIGHT MOTOR
pinMode(22,OUTPUT); //ART 3 STEP BELT MOTOR
pinMode(23,OUTPUT); //ART 4 STEP HEAD BASE MOTOR
pinMode(25,OUTPUT); //ART 56 STEP LEFT MOTOR
pinMode(27,OUTPUT); //ART 56 STEP RIGHT MOTOR

//DIRECTION OUTPUTS--------------------------------
pinMode(36,OUTPUT); //ART 1 DIR BASE
pinMode(34,OUTPUT); //ART 2 DIR LEFT MOTOR
pinMode(32,OUTPUT); //ART 2 DIR RIGHT MOTOR
pinMode(30,OUTPUT); //ART 3 DIR BELT MOTOR
pinMode(31,OUTPUT); //ART 4 DIR HEAD BASE MOTOR
pinMode(33,OUTPUT); //ART 56 DIR LEFT MOTOR
pinMode(35,OUTPUT); //ART 56 DIR RIGHT MOTOR

//CLAW OUTPUT-------------------------------------
pinMode(7,OUTPUT); //CLAW PWM OUTPUT 5V



}

void loop() {

//this small section is useful to test the functionality of the opto sensors

Serial.println("Gladys says: ");
Serial.print("Art1: ");
Serial.println(digitalRead(42));
Serial.print("Art3: ");
Serial.println(digitalRead(48));
Serial.print("Art4: ");
Serial.println(digitalRead(49));
Serial.print("SWITCH: ");
Serial.println(digitalRead(47));

delay(1000);
auto aval = Serial.available();
if (aval > 0 ){
auto input = Serial.readStringUntil('\r');
auto read = input.length();
while(aval > ++read)
Serial.read();

auto incomingByte=input.toInt();

Serial.print("Received data is: ");
Serial.println(incomingByte);



switch (incomingByte){

  case 1: //HOME POSITION
  Serial.println("HOMIE POSITION");
 // Serial.println("Major Tom is going HOME position");

//TEST THEM ONE BY ONE

  //something that can be added to the 'home' function is a parameter to increase/decrease the steps (FOR loops) 
  //and also how fast the movement is (set up the delays with a constant variable)
  // STEP, DIR, OPTO
 home1Motor(28,36,42);// delay(2500); //this works for articulation 1 (rotating base) 
  //I haven't defined code for articulation 2 as it's not working properly - 2 motors at the same time
 home1Motor(22,30,48);// delay(2500);//this works for articulation 3 (open belt) - currently working perfectly. The belt and the motor hold the weight of the head
 home1Motor(23,31,49);// delay(2500);//this works for articulation 4 (base of the head)
  //for all 3 'home' positions, only one motor is used to perform the movement
  //now I have to do a separate function for the head as it uses a endstop switch to set its initial position
  home2motors_head(25,33,47); // makes contact with the switch and then returns back the base of the claw to the middle of the head
  home_claw_column(25,33,27,35,47); //rotates the base of the claw a certain number of degrees based on the number of steps
                                    //needs hard testing to be accurate and a base position to start with
  break;

  case 2: //AUTO MODE
  Serial.println("AUTO MODE");
 // Serial.println("Transformers: AUTO-robots in disguise");
  break;

  case 3: //MANUAL MODE
  Serial.println("MANUAL MODE");
 // Serial.println("2003 Mazda Protege5 Standard/MANUAL transmission");

  HOME_MODE(); //set the robotic arm at home position first so I can count the steps going left or right (DIR pin)

  int a_home_value=0;

  int a_CW=manual1MotorCW(28,36); //articulation 1 base CW
  a_home_value=a_home_value+a_CW;

  int a_CCW=manual1MotorCCW(28,36); //articulation 1 base CCW
  a_home_value=a_home_value+a_CCW;


  //int b=manual1Motor(22,30); //articulation 3 open belt
  //int c=manual1Motor(23,31); //articulation 4 head base 
  
  //still need to work on how to manually control articulation 56 for both type of movements Nov 22nd 2:29PM
  
  break;

  default:
  
  break;

}
}
}

void home1Motor(const int STEP,const int DIR,const int OPTO)
{
/*
  Serial.println("--------------");
  Serial.println(STEP);
  Serial.println(DIR);
  Serial.println(OPTO);
*/
      Serial.println("the motor is just about to rotate CW");
      delay(500);
      digitalWrite(DIR,HIGH); //MOTOR GOES CW

      for(int i=0;i<14000;i++){ //1035 was gud for a full rotation
      //I think I was using full step mode

      if(digitalRead(OPTO)==LOW){
        Serial.println("the sensor detected a notch after turning CW");
        //i=0;
        digitalWrite(DIR,LOW);
        break;        
      }

      digitalWrite(STEP,HIGH);
      delayMicroseconds(100); //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
      digitalWrite(STEP,LOW); //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
      delayMicroseconds(400);

      //delay(1); //this variable could be controlled

      }
      if(digitalRead(OPTO)==HIGH){
        Serial.println("the sensor didn't detect a notch");
        
        digitalWrite(DIR,LOW);
        Serial.println("the motor is just about to rotate CCW");
        delay(500);
        for(int j=0;j<20000;j++){ //1035 was gud for a full rotation
        //I think I was using full step mode
        digitalWrite(STEP,HIGH);
        delayMicroseconds(100);
        digitalWrite(STEP,LOW);
        delayMicroseconds(400);

        //delay(1);
        if(digitalRead(OPTO)==LOW){
        Serial.println("the sensor detected a notch after turning CCW");
        //j=0;
        break;
        }

        }// im using two breaks for both senses of rotation

        
}
        Serial.print("Sensor value is: ");
        Serial.println(digitalRead(OPTO));
        Serial.println("If zero, the notch was found. If one, the notch wasn't found");
        Serial.println("the end");
}
int home2motors_head(int STEP, int DIR, int SW) //this moves the claw support from left to right until it reaches the switch
                                                //after that, it returns the claw column back to the middle position based on the number of steps 
                                                //set at the for loop
{
      Serial.println("the motor is just about to rotate CW");
      delay(500);
      digitalWrite(DIR,LOW); //MOTOR GOES CW
      for(int i=0;i<10000;i++)
      { //1035 was gud for a full rotation
      //I think I was using full step mode
   //int inc;
     // inc++;
      //Serial.println(inc);
      if(digitalRead(SW)==HIGH){
      Serial.println("The switch was found!!"); //as soon as the switch is pressed by the claw column, the claw goes back to the mid section of the robotic arm head
      
      digitalWrite(DIR,HIGH);
      break;

      }
      digitalWrite(STEP,HIGH);
      delayMicroseconds(100); //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
      digitalWrite(STEP,LOW); //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
      delayMicroseconds(400);
      
      //delay(1); //this variable could be controlled


      }
      digitalWrite(DIR,HIGH); //MOTOR GOES CCW
      for(int i=0;i<3200;i++){ //1035 was gud for a full rotation
      //I think I was using full step mode
      digitalWrite(STEP,HIGH);
      delayMicroseconds(100); //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
      digitalWrite(STEP,LOW); //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
      delayMicroseconds(400);
      //int inc;
     // inc++;
      //Serial.println(inc);
      //delay(1); //this variable could be controlled
      }
      Serial.println("the base of the claw has returned to the middle");
}


int home_claw_column(int STEP1, int DIR1, int STEP2, int DIR2, int SW)
{
        Serial.println("the claw is about to rotate on its own axis");
        //delay(1000);
        digitalWrite(DIR1,LOW);
        digitalWrite(DIR2,HIGH);
        for(int i=0;i<12800;i++){
        
        if(digitalRead(SW)==HIGH){
        Serial.println("somehow I pressed the switch");
        
        digitalWrite(DIR1,LOW);
        digitalWrite(DIR2,LOW);
        break;
        }
        if(digitalRead(SW)==LOW){
        

        digitalWrite(STEP1,HIGH);
        delayMicroseconds(100);
        digitalWrite(STEP1,LOW);
        delayMicroseconds(500);

        digitalWrite(STEP2,HIGH);
        delayMicroseconds(100);
        digitalWrite(STEP2,LOW);
        delayMicroseconds(500);
        }

      }
      Serial.println("the claw has rotated");
}

int manual1MotorCW(int STEP, int DIR){
        digitalWrite(DIR,HIGH);
        int count_1M=0;
        for(int i=0;i>100;i++){
        digitalWrite(STEP,HIGH);
        delayMicroseconds(250);
        digitalWrite(STEP,LOW);
        delayMicroseconds(750);
        delay(10);
        count_1M+100;
        }
      return count_1M;
}

int manual1MotorCCW(int STEP, int DIR){
        digitalWrite(DIR,LOW);
        int count_1M=0;
        for(int i=0;i>100;i++){
        digitalWrite(STEP,HIGH);
        delayMicroseconds(250);
        digitalWrite(STEP,LOW);
        delayMicroseconds(750);
        delay(10);
        count_1M-100;
        }
      return count_1M;
}



void HOME_MODE(){

home1Motor(28,36,42); //art 1
home1Motor(22,30,48); //art 3
home1Motor(23,31,49); //art 4
home2motors_head(25,33,47); //art56 left to right movement 
home_claw_column(25,33,27,35,47); // rotation of the claw column

}

