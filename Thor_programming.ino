//All these motors are connected to a bipolar stepper motor driver A4988. We use only the STEP and DIR pins (control pins), 12V motors
//All inputs work with 5V
//The claw(servo) works with 5V
#include <Servo.h>
Servo CLAW;

//INPUTS PIN DEFINITION
const int Opto_Art1 = 42;
const int Opto_Art2 = 44;
const int Opto_Art3 = 48;
const int Opto_Art4 = 49;
const int SW_Art56 = 47;

//OUTPUTS PIN DEFINITION
const int Art1_STEP = 28;
const int Art1_DIR = 36;

const int Art2_Left_STEP = 26;
const int Art2_Left_DIR = 34;

const int Art2_Right_STEP = 24;
const int Art2_Right_DIR = 32;

const int Art3_Belt_STEP = 22;
const int Art3_Belt_DIR = 30;

const int Art4_STEP = 23;
const int Art4_DIR = 31;

const int Art56_Left_STEP = 25;
const int Art56_Left_DIR = 33;

const int Art56_Right_STEP = 27;
const int Art56_Right_DIR = 35;

const int Claw = 7;
const int CLAW_pos = 0;

//COUNTING STEPS VARIABLES

int art1_steps_count = 0;  //all these variables count steps from -32,768 to 32,767 (int datatype range)
int art3_steps_count = 0;
int art4_steps_count = 0;
int art5_steps_count = 0;
int art6_steps_count = 0;

void setup() {
  //Code that runs once
  CLAW.attach(7);
  Serial.begin(9600);
  while (!Serial)
    ;

  //INPUTS PIN DEFINITION
  //SENSORS INPUTS
  pinMode(42, INPUT);  //Art1 Optodisk 5V
  pinMode(44, INPUT);  //Art2 Optodisk 5V
  pinMode(48, INPUT);  //Art3 LOWER Optodisk 5V
  pinMode(49, INPUT);  //Art4 UPPER Optodisk 5V
  pinMode(47, INPUT);  //Art56 Endstop Switch 5V

  //STEP OUTPUTS-------------------------------------
  pinMode(28, OUTPUT);  //ART 1 STEP BASE
  pinMode(26, OUTPUT);  //ART 2 STEP LEFT MOTOR
  pinMode(24, OUTPUT);  //ART 2 STEP RIGHT MOTOR
  pinMode(22, OUTPUT);  //ART 3 STEP BELT MOTOR
  pinMode(23, OUTPUT);  //ART 4 STEP HEAD BASE MOTOR
  pinMode(25, OUTPUT);  //ART 56 STEP LEFT MOTOR
  pinMode(27, OUTPUT);  //ART 56 STEP RIGHT MOTOR

  //DIRECTION OUTPUTS--------------------------------
  pinMode(36, OUTPUT);  //ART 1 DIR BASE
  pinMode(34, OUTPUT);  //ART 2 DIR LEFT MOTOR
  pinMode(32, OUTPUT);  //ART 2 DIR RIGHT MOTOR
  pinMode(30, OUTPUT);  //ART 3 DIR BELT MOTOR
  pinMode(31, OUTPUT);  //ART 4 DIR HEAD BASE MOTOR
  pinMode(33, OUTPUT);  //ART 56 DIR LEFT MOTOR
  pinMode(35, OUTPUT);  //ART 56 DIR RIGHT MOTOR

  //CLAW OUTPUT-------------------------------------
  //pinMode(7, OUTPUT);  //CLAW PWM OUTPUT 5V
}

void loop() {

  //this small section is useful to test the functionality of the opto sensors and switc
  /*
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
   
  Serial.println("art1: ");
  Serial.println(art1_steps);
  Serial.println("art3: ");
  Serial.println(art3_steps_count);
  Serial.println("art4: ");
  Serial.println(art4_steps_count);
  Serial.println("art5: ");
  Serial.println(art5_steps_count);
  Serial.println("art6: ");
  Serial.println(art6_steps_count);
  delay(1000);
*/

  auto aval = Serial.available();

  if (aval > 0) {
    auto input = Serial.readStringUntil('\r');
    auto read = input.length();
    while (aval > ++read)
      Serial.read();
    //CLAW.write(45);
    auto incomingByte = input.toInt();

    switch (incomingByte) {

      case 1:  //HOME POSITION
        Serial.println("HOMIE POSITION");
        //Serial.println("Major Tom is going HOME position");
        //It can be tested in group

        //something that can be added to the 'home' function is a parameter to increase/decrease the steps (FOR loops)
        //and also how fast the movement is (set up the delays with a constant variable)
        // STEP, DIR, OPTO
        home1Motor(28, 36, 42);  // delay(2500); //this works for articulation 1 (rotating base)
                                 //I haven't defined code for articulation 2 as it's not working properly - 2 motors at the same time
        home1Motor(22, 30, 48);  // delay(2500);//this works for articulation 3 (open belt) - currently working perfectly. The belt and the motor hold the weight of the head
        home1Motor(23, 31, 49);  // delay(2500);//this works for articulation 4 (base of the head)
        //for all 3 'home' positions, only one motor is used to perform the movement
        //now I have to do a separate function for the head as it uses a endstop switch to set its initial position
        home2motors_head(25, 33, 47);         // makes contact with the switch and then returns back the base of the claw to the middle of the head
        claw_column_CCW(25, 33, 27, 35, 47);  //rotates the base of the claw a certain number of degrees based on the number of steps
                                              //needs hard testing to be accurate and a base position to start with



        Serial.println("HOME MODE WAS REACHED!");
        claw_game();

        break;

      case 2:                         //AUTO MODE
        Serial.println("AUTO MODE");  //Need to wait until the swivel is finished
                                      // Serial.println("Transformers: AUTO-robots in disguise");
        break;

      case 3:  //MANUAL MODE
        Serial.println("WELCOME TO MANUAL MODE");
        Serial.println("PRESS ONE OF THE FOLLOWING OPTIONS: ");

        Serial.println("11 - Articulation 1 moves to the right ");
        Serial.println("12 - Articulation 1 moves to the left ");

        Serial.println("31 - Articulation 3 moves to the right ");
        Serial.println("32 - Articulation 3 moves to the left ");

        Serial.println("41 - Articulation 4 moves to the right ");
        Serial.println("42 - Articulation 4 moves to the left ");

        Serial.println("51 - Articulation 5 moves to the right ");
        Serial.println("52 - Articulation 5 moves to the left ");

        Serial.println("61 - Articulation 6 moves CW ");
        Serial.println("62 - Articulation 6 moves CCW ");

        Serial.println("71 - Control the Claw! ");

        Serial.println("4 - Return to MAIN MENU");
        while (true) {

          auto aval2 = Serial.available();
          if (aval2 > 0) {

            auto input2 = Serial.readStringUntil('\r');
            auto read2 = input2.length();
            while (aval2 > ++read2)
              Serial.read();

            auto incomingByte2 = input2.toInt();

            switch (incomingByte2) {

              case 11:
                Serial.println("Articulation 1 moving to the right");
                manual1MotorHIGH(28, 36, 2000);
                break;
              case 12:
                Serial.println("Articulation 1 moving to the left");
                manual1MotorLOW(28, 36, 2000);
                break;

              case 31:
                Serial.println("Articulation 3 moving to the right");
                manual1MotorHIGH(22, 30, 2000);
                break;

              case 32:
                Serial.println("Articulation 3 moving to the left");
                manual1MotorLOW(22, 30, 2000);
                break;

              case 41:
                Serial.println("Articulation 4 moving to the left");
                manual1MotorHIGH(23, 31, 2000);
                break;

              case 42:
                Serial.println("Articulation 4 moving to the right");
                manual1MotorLOW(23, 31, 2000);
                break;

              case 51:
                Serial.println("Articulation 5 moving to the left");
                manual1MotorLOW(25, 33, 500);
                break;

              case 52:
                Serial.println("Articulation 5 moving to the right");
                manual1MotorHIGH(25, 33, 500);
                break;

              case 61:
                Serial.println("Articulation 6 rotating clockwise");
                manual2MotorHIGH(25, 33, 27, 35, 1000);
                break;

              case 62:
                Serial.println("Articulation 6 rotation counterclockwise");
                manual2MotorLOW(25, 33, 27, 35, 1000);
                break;
              case 71:
                Serial.println("Thanks for using the THUNDERCLAW");
                Serial.println("Type values between 0 and 90");
                Serial.println("When at 90, it's fully OPEN and at 0 it's fully CLOSED");
                Serial.println("If you enter a value beyond or below the limits, you will go back to MANUAL MODE");
                manualCLAW(90);
                break;

              case 4:
                Serial.println("Exiting to main menu...");
                break;

              default:

                Serial.println("Wrong Input. This is a friendly reminder for the available options:");
                Serial.println("11 - Articulation 1 moves to the right ");
                Serial.println("12 - Articulation 1 moves to the left ");

                Serial.println("31 - Articulation 3 moves to the right ");
                Serial.println("32 - Articulation 3 moves to the left ");

                Serial.println("41 - Articulation 4 moves to the right ");
                Serial.println("42 - Articulation 4 moves to the left ");

                Serial.println("51 - Articulation 5 moves to the right ");
                Serial.println("52 - Articulation 5 moves to the left ");

                Serial.println("61 - Articulation 6 moves CW ");
                Serial.println("62 - Articulation 6 moves CCW ");

                Serial.println("71 - Control the Claw! ");

                Serial.println("4 - Return to MAIN MENU");
                break;
            }
            //            Serial.println(incomingByte2);
            if (incomingByte2 == 4) {  //exit keybind
              Serial.println("Welcome back to the main menu. These are the available options: ");
              Serial.println("1 - HOME");
              Serial.println("2 - AUTO");
              Serial.println("3 - MANUAL");
              break;
            }
          }
        }
        break;

      case 4:

        Serial.println("tests");
        //HOME_MODE();

       moveit(0, 0, 0, 0, 6400);
       returnHOME(28, 36, 22, 30, 23, 31, 25, 33, 27, 35);
       
        moveit(0, 0, 0, 0, -6400);
        returnHOME(28, 36, 22, 30, 23, 31, 25, 33, 27, 35);
        
          /*
        CLAW.write(90);
         moveit(-1000, 6000,1000, 1500, 0);
         delay(500);
         CLAW.write(45);
         delay(500);

         moveit(-1000, -8000, 1000, -500, 0);
         delay(1000);
         CLAW.write(90);
*/


          // moveit(1600, 4283, -6000, -1500, 2000);


          /*
        home1Motor(28, 36, 42);        //art 1
        home1Motor(22, 30, 48);        //art 3
        home1Motor(23, 31, 49);        //art 4
        home2motors_head(25, 33, 47);  //art 5
        claw_column_CCW(25, 33, 27, 35, 47);
        claw_column_CW(25, 33, 27, 35, 47);
        claw_game();
        /*
home1Motor(28, 36, 42);  //art 1
home1Motor(22, 30, 48);  //art 3
home1Motor(23, 31, 49);  //art 4
home2motors_head(25, 33, 47); //art 5

//home2motors_head(27, 35, 47); //art 5

claw_column_CCW(25, 33, 27, 35, 47);
delay(2500);
claw_column_CW(25, 33, 27, 35, 47);

        /*
                art1_steps_count=0;
        art3_steps_count=0;
        art4_steps_count=0;
        art5_steps_count=0;
        art6_steps_count=0;
        home1Motor(28, 36, 42);  //art 1

        home1Motor(22, 30, 48);  //art 3

        home1Motor(23, 31, 49);  //art 4

        home2motors_head(25, 33, 47);  //art56 left to right movement

        
        
        art1_steps_count=0;
        art3_steps_count=0;
        art4_steps_count=0;
        art5_steps_count=0;
        art6_steps_count=0;


        delay(2000);
*/

          //moveit(-1300, -2200, 1500, 2000, -4000);

          // claw_game();
          // moveit(1600, 4283, 6000, -3600, 5000);
          //delay(1000);
          // returnHOME(28, 36, 22, 30, 23, 31, 25, 33, 27, 35);

          //moveit(2500, 2000, 1000, -2000, 1000);

          /*
        delay(2000);
        returnHOME(28, 36, 22, 30, 23, 31, 25, 33);
        delay(2000);
        moveit(2500, 2000, 1000, -2000,1000);
        delay(2000);
        returnHOME(28, 36, 22, 30, 23, 31, 25, 33);
*/


          //claw_game();


          // returnHOME(28, 36, 22, 30, 23, 31, 25, 33);
          // delay(2000);





          //Serial.println("I just moved it to pos 1");
          //delay(1000);
          //moveit(-2500, -2000, -1000, -1000);
          //Serial.println("I just moved it to pos 2");

          // HOME_MODE();
          // claw_game();*/
          break;

      case 5:
        //Claw_pos can only go from 0 to 90. Even better from 5 to 85 just to avoid friction or forceful movements
        claw_game();
        break;

      default:
        Serial.println("Wrong user input. For future reference here's the available modes:");
        Serial.println("1 - HOME");
        Serial.println("2 - AUTO");
        Serial.println("3 - MANUAL");
        break;
    }
  }
}


//FROM HERE YOU WILL ONLY SEE FUNCTIONS THAT THE MAIN LOOP USES

void home1Motor(const int STEP, const int DIR, const int OPTO) {
  /*
  Serial.println("--------------");  Serial.println(STEP);  Serial.println(DIR);  Serial.println(OPTO);
*/
  Serial.println("the motor is just about to rotate CW");
  delay(500);
  digitalWrite(DIR, HIGH);  //MOTOR GOES CW

  for (int i = 0; i < 14000; i++) {  //1035 was gud for a full rotation
    //I think I was using full step mode

    if (digitalRead(OPTO) == LOW) {
      Serial.println("the sensor detected a notch after turning CW");
      //i=0;
      digitalWrite(DIR, LOW);
      break;
    }
    if (STEP == 28) { art1_steps_count++; }
    if (STEP == 22) { art3_steps_count++; }
    if (STEP == 23) { art4_steps_count++; }
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);   //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
    digitalWrite(STEP, LOW);  //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
    delayMicroseconds(400);


    //delay(1); //this variable could be controlled
  }
  if (digitalRead(OPTO) == HIGH) {
    Serial.println("the sensor didn't detect a notch");

    digitalWrite(DIR, LOW);
    Serial.println("the motor is just about to rotate CCW");
    delay(500);
    for (int j = 0; j < 20000; j++) {  //1035 was gud for a full rotation
                                       //I think I was using full step mode
      if (STEP == 28) { art1_steps_count--; }
      if (STEP == 22) { art3_steps_count--; }
      if (STEP == 23) { art4_steps_count--; }
      digitalWrite(STEP, HIGH);
      delayMicroseconds(100);
      digitalWrite(STEP, LOW);
      delayMicroseconds(400);

      //delay(1);
      if (digitalRead(OPTO) == LOW) {
        Serial.println("the sensor detected a notch after turning CCW");

        break;
      }

    }  // im using two breaks for both senses of rotation
  }
  Serial.print("Sensor value is: ");
  Serial.println(digitalRead(OPTO));
  if (digitalRead(OPTO) == 0) {
    Serial.println("The notch was found!");
  } else {
    Serial.println("I didn't find the notch :(");
  }
  Serial.println("-------------------------------------------------------------------------");
  Serial.println("Art1_steps: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-------------------------------------------------------------------------");

  Serial.println("Next sequence is about to start");
}
int home2motors_head(int STEP, int DIR, int SW)  //this moves the claw column from left to right until it reaches the switch
                                                 //after that, it returns the claw column back to the middle position
{
  delay(100);
  digitalWrite(DIR, HIGH);           //MOTOR GOES CCW
  for (int i = 0; i < 10000; i++) {  //1035 was gud for a full rotation

    if (digitalRead(SW) == HIGH) {
      Serial.println("The switch was found!!");  //as soon as the switch is pressed by the claw column, the claw goes back to the mid section of the robotic arm head

      digitalWrite(DIR, LOW);
      break;
    }
    if (STEP == 25) { art5_steps_count--; }
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);   //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
    digitalWrite(STEP, LOW);  //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
    delayMicroseconds(350);

    //delay(1); //this variable could be controlled
  }
  digitalWrite(DIR, LOW);           //MOTOR GOES CCW
  for (int i = 0; i < 3200; i++) {  //1035 was gud for a full rotation
    //I think I was using full step mode
    if (STEP == 25) { art5_steps_count++; }
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);   //these two delays could be controlled. Right now D=0.25 and period is 1ms. f=1khz
    digitalWrite(STEP, LOW);  //including the dead time of 10ms, 1ms +10ms is 11ms so period is 11ms. f=90.9
    delayMicroseconds(350);
  }
  Serial.println("the base of the claw has returned to the middle");
  Serial.println("-------------------------------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-------------------------------------------------------------------------");
}


int claw_column_CCW(int STEP1, int DIR1, int STEP2, int DIR2, int SW) {  //rotation to the left on its own axis CCW
  Serial.println("the claw is about to rotate left on its own axis");
  //delay(1000);
  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
  digitalWrite(STEP1, LOW);
  digitalWrite(STEP2, LOW);
  for (int i = 0; i < 6400; i++) {

    if (digitalRead(SW) == HIGH) {
      digitalWrite(DIR1, LOW);
      digitalWrite(DIR2, LOW);
      Serial.println("somehow I pressed the switch");
      break;
    }

    if (digitalRead(SW) == LOW) {
      art6_steps_count++;
      digitalWrite(STEP1, HIGH);
      delayMicroseconds(100);
      digitalWrite(STEP1, LOW);
      delayMicroseconds(350);

      digitalWrite(STEP2, HIGH);
      delayMicroseconds(100);
      digitalWrite(STEP2, LOW);
      delayMicroseconds(350);
    }
  }
  Serial.println("the claw has rotated");
  Serial.println("-------------------------------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-------------------------------------------------------------------------");
}


int claw_column_CW(int STEP1, int DIR1, int STEP2, int DIR2, int SW) {  //rotation to the right on its own axis CW
  Serial.println("the claw is about to rotate right on its own axis");
  //delay(1000);
  digitalWrite(DIR1, HIGH);  //original CCW had DIR1 AS LOW AND DIR2 AS HIGH
  digitalWrite(DIR2, LOW);   //This is done to change the direction of movement of the claw column
  digitalWrite(STEP1, LOW);
  digitalWrite(STEP2, LOW);

  for (int i = 0; i < 6400; i++) {

    if (digitalRead(SW) == HIGH) {
      digitalWrite(DIR1, LOW);
      digitalWrite(DIR2, LOW);
      Serial.println("somehow I pressed the switch");
      break;
    }

    if (digitalRead(SW) == LOW) {
      art6_steps_count--;
      digitalWrite(STEP1, HIGH);
      delayMicroseconds(100);
      digitalWrite(STEP1, LOW);
      delayMicroseconds(350);

      digitalWrite(STEP2, HIGH);
      delayMicroseconds(100);
      digitalWrite(STEP2, LOW);
      delayMicroseconds(350);
    }
  }
  Serial.println("the claw has rotated");
  Serial.println("-------------------------------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-------------------------------------------------------------------------");
}


void HOME_MODE() {  //gathers all the home position commands into one function

  home1Motor(28, 36, 42);        //art 1
  home1Motor(22, 30, 48);        //art 3
  home1Motor(23, 31, 49);        //art 4
  home2motors_head(25, 33, 47);  //art56 left to right movement
  //claw_column_CCW(25, 33, 27, 35, 47);  // rotation of the claw column to the left
  //claw_column_CW(25, 33, 27, 35, 47);   // rotation of the claw column to the right
}

void moveit(int amount_of_steps_1, int amount_of_steps_2, int amount_of_steps_3, int amount_of_steps_4, int amount_of_steps_5) {

  int STEP_NUMBERS[5] = { amount_of_steps_1, amount_of_steps_2, amount_of_steps_3, amount_of_steps_4, amount_of_steps_5 };
  int STEPS_PER_MOTOR[5] = { amount_of_steps_1, amount_of_steps_2, amount_of_steps_3, amount_of_steps_4, amount_of_steps_5 };

  int n = sizeof(STEP_NUMBERS) / sizeof(STEP_NUMBERS[0]);


  bubbleSort(STEP_NUMBERS, n);

  for (int i = 0; i < n; i++) {
    Serial.print(STEP_NUMBERS[i]);
    Serial.print(" ");
  }
  Serial.println(" ");


  parallel_motors(STEP_NUMBERS, STEPS_PER_MOTOR, 28, 36, 22, 30, 23, 31, 25, 33, 27, 35);  //added STEP 27 and DIR 35
}
void bubbleSort(int arr[], int n) {  //bubble sorting algorithm

  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        // swap arr[j] and arr[j+1]
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
}

void parallel_motors(int array[], int steps[],
                     const int step_a, const int dir_a,
                     const int step_b, const int dir_b,
                     const int step_c, const int dir_c,
                     const int step_d, const int dir_d,
                     const int step_e, const int dir_e)

{
  if ((array[0] == 0) && (array[1] == 0) && (array[2] == 0) && (array[3] == 0) && (array[4] == 0)) {
    Serial.println("no valid values detected");
    return;
  }

  int max_step = 0;
  int mod_array[5];

  if (steps[0] >= 0) {
    digitalWrite(dir_a, HIGH);
  } else {
    digitalWrite(dir_a, LOW);
  }

  if (steps[1] >= 0) {
    digitalWrite(dir_b, HIGH);
  } else {
    digitalWrite(dir_b, LOW);
  }

  if (steps[2] >= 0) {
    digitalWrite(dir_c, HIGH);
  } else {
    digitalWrite(dir_c, LOW);
  }







  Serial.println("ORIGINAL SORTING ARRAY - LOWER TO GREATER");
  Serial.print("first #: ");
  Serial.println(array[0]);
  Serial.print("second #: ");
  Serial.println(array[1]);
  Serial.print("third #: ");
  Serial.println(array[2]);
  Serial.print("fourth #: ");
  Serial.println(array[3]);
  Serial.print("fifth #: ");
  Serial.println(array[4]);



  Serial.println("MODIFIED SORTING ARRAY - LOWER TO GREATER FOR NEGATIVE NUMBERS");
  mod_array[0] = abs(array[0]);
  mod_array[1] = abs(array[1]);
  mod_array[2] = abs(array[2]);
  mod_array[3] = abs(array[3]);
  mod_array[4] = abs(array[4]);

  int ABS_STEP_NUMBERS[5] = { mod_array[0], mod_array[1], mod_array[2], mod_array[3], mod_array[4] };

  int n = sizeof(ABS_STEP_NUMBERS) / sizeof(ABS_STEP_NUMBERS[0]);
  bubbleSort(ABS_STEP_NUMBERS, n);
  Serial.println("///////////////////////////////////////// ");
  for (int i = 0; i < n; i++) {
    Serial.print(ABS_STEP_NUMBERS[i]);
    Serial.print(" ");
  }
  Serial.println(" ");

  Serial.println("///////////////////////////////////////// ");


  Serial.println("STEPS PER MOTOR - ENTERED WITH MOVEIT FN");
  Serial.print("first #: ");
  Serial.println(steps[0]);
  Serial.print("second #: ");
  Serial.println(steps[1]);
  Serial.print("third #: ");
  Serial.println(steps[2]);
  Serial.print("fourth #: ");
  Serial.println(steps[3]);
  Serial.print("fifth #: ");
  Serial.println(steps[4]);


  max_step = ABS_STEP_NUMBERS[4];
  Serial.println("Max value in the STEP number array is: ");
  Serial.println(ABS_STEP_NUMBERS[4]);
  int a = 0, b = 0, c = 0, d = 0, e = 0;
  int art1 = 0, art3 = 0, art4 = 0, art5 = 0, art6 = 0;

  digitalWrite(step_a, LOW);
  digitalWrite(step_b, LOW);
  digitalWrite(step_c, LOW);
  digitalWrite(step_d, LOW);
  digitalWrite(step_e, LOW);

  for (int i = 0; i <= max_step; i++) {
    //-----------------------------------------------------------------------------------------ART1
    if ((abs(a) <= abs(steps[0]) && (art1 == 0) && (steps[0] != 0))) {
      if (steps[0] >= 0) {
        a++;
      } else {
        a--;
      }
      digitalWrite(step_a, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_a, LOW);
      delayMicroseconds(350);
    }

    if (abs(a) == abs(steps[0])) {
      art1 = 1;
    }

    //-----------------------------------------------------------------------------------------ART3
    if ((abs(b) <= abs(steps[1]) && (art3 == 0) && (steps[1] != 0))) {
      if (steps[1] >= 0) {
        b++;
      } else {
        b--;
      }
      digitalWrite(step_b, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_b, LOW);
      delayMicroseconds(350);
    }

    if (abs(b) == abs(steps[1])) {
      art3 = 1;
    }
    //-----------------------------------------------------------------------------------------ART4
    if ((abs(c) <= abs(steps[2])) && (art4 == 0) && (steps[2] != 0)) {
      if (steps[2] >= 0) {
        c++;
      } else {
        c--;
      }
      digitalWrite(step_c, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_c, LOW);
      delayMicroseconds(350);
    }

    if (abs(c) == abs(steps[2])) {
      art4 = 1;
    }
    //-----------------------------------------------------------------------------------------ART5

    if (steps[3] >= 0) {
      digitalWrite(dir_d, HIGH);
    } else {
      digitalWrite(dir_d, LOW);
    }

    if ((abs(d) <= abs(steps[3]) && (art5 == 0) && (steps[3] != 0))) {
      if (digitalRead(SW_Art56) == 0) {
        if (steps[3] >= 0) {
          d++;
        } else {
          d--;
        }
        digitalWrite(step_d, HIGH);
        delayMicroseconds(100);
        digitalWrite(step_d, LOW);
        delayMicroseconds(350);
      }
    }

    if (abs(d) == abs(steps[3])) {
      art5 = 1;
    }
    //-----------------------------------------------------------------------------------------ART 6 - ROTATION OF THE CLAW COLUMN

    if (steps[4] >= 0) {
      digitalWrite(dir_d, HIGH);
      digitalWrite(dir_e, LOW);
    } else {
      digitalWrite(dir_d, LOW);
      digitalWrite(dir_e, HIGH);
    }

    if ((abs(e) <= abs(steps[4]) && (art6 == 0) && (steps[4] != 0))) {
      if (digitalRead(SW_Art56) == 0) {
        if (steps[4] >= 0) {
          e++;
        } else {
          e--;
        }  ////////////

        digitalWrite(step_d, HIGH);
        delayMicroseconds(100);
        digitalWrite(step_d, LOW);
        delayMicroseconds(350);

        digitalWrite(step_e, HIGH);
        delayMicroseconds(100);
        digitalWrite(step_e, LOW);
        delayMicroseconds(350);
      }
    }

    if (abs(e) == abs(steps[4])) {

      art6 = 1;
    }
  }

  //-----------------------------------------------------------------------------------------END OF ARTICULATIONS

  //-----------------------------------------PRINTING THE RESULTS--------------------------------------------------
  Serial.println("-------------BEFORE THE SUM-------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);

  Serial.println("-----------------------------------------------------------------------------");

  art1_steps_count = art1_steps_count + a;
  art3_steps_count = art3_steps_count + b;
  art4_steps_count = art4_steps_count + c;
  art5_steps_count = art5_steps_count + d;
  art6_steps_count = art6_steps_count + e;

  Serial.println("-------------AFTER THE SUM---------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-----------------------------------------------------------------------------");

  Serial.println("a/b/c/d values added: ");
  Serial.print("a: ");
  Serial.println(a);
  Serial.print("b: ");
  Serial.println(b);
  Serial.print("c: ");
  Serial.println(c);
  Serial.print("d: ");
  Serial.println(d);
  Serial.print("e: ");
  Serial.println(e);

  if (art1 == 1 && art3 == 1 && art4 == 1 && art5 == 1 && art6 == 1) {
    Serial.println("MISSION COMPLETE");
  }
}


void claw_game() {
  //the claw dancing like it should

  CLAW.write(5);
  delay(250);
  CLAW.write(15);
  delay(250);
  CLAW.write(25);
  delay(250);
  CLAW.write(35);
  delay(250);
  CLAW.write(45);
  delay(250);
  CLAW.write(55);
  delay(250);
  CLAW.write(65);
  delay(250);
  CLAW.write(75);
  delay(250);
  CLAW.write(85);
  delay(250);

  for (int CLAW_pos = 5; CLAW_pos <= 85; CLAW_pos += 1) {
    CLAW.write(CLAW_pos);
    delay(25);
  }

  for (int CLAW_pos = 85; CLAW_pos >= 5; CLAW_pos -= 1) {
    CLAW.write(CLAW_pos);
    delay(25);
  }

  CLAW.write(45);
}

void returnHOME(const int step_a, const int dir_a,
                const int step_b, const int dir_b,
                const int step_c, const int dir_c,
                const int step_d, const int dir_d,
                const int step_e, const int dir_e) {

  int a = 0, b = 0, c = 0, d = 0, e = 0;

  digitalWrite(step_a, LOW);
  digitalWrite(step_b, LOW);
  digitalWrite(step_c, LOW);
  digitalWrite(step_d, LOW);
  digitalWrite(step_e, LOW);

  Serial.println("Initial position of returnHOME: ");
  Serial.println("1st art steps is: ");
  Serial.println(art1_steps_count);
  Serial.println("3rd art steps is: ");
  Serial.println(art3_steps_count);
  Serial.println("4th art steps is: ");
  Serial.println(art4_steps_count);
  Serial.println("5th art steps is: ");
  Serial.println(art5_steps_count);
  Serial.println("6th art steps is: ");
  Serial.println(art6_steps_count);
  /////////////////////////////////////////////////////////ART1
  if (art1_steps_count >= 0) {
    digitalWrite(dir_a, LOW);
    for (int i = 0; i < art1_steps_count; i++) {

      if (art1_steps_count == 0) {
        break;
      }
      a--;
      digitalWrite(step_a, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_a, LOW);
      delayMicroseconds(350);
    }
  }

  if (art1_steps_count < 0) {
    digitalWrite(dir_a, HIGH);
    for (int i = 0; i > art1_steps_count; i--) {
      a++;
      digitalWrite(step_a, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_a, LOW);
      delayMicroseconds(350);
    }
  }
  /////////////////////////////////////////////////////////ART3
  if (art3_steps_count >= 0) {
    digitalWrite(dir_b, LOW);
    for (int i = 0; i < art3_steps_count; i++) {

      if (art3_steps_count == 0) {
        break;
      }
      b--;
      digitalWrite(step_b, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_b, LOW);
      delayMicroseconds(350);
    }
  }
  if (art3_steps_count < 0) {
    digitalWrite(dir_b, HIGH);
    for (int i = 0; i > art3_steps_count; i--) {
      b++;
      digitalWrite(step_b, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_b, LOW);
      delayMicroseconds(350);
    }
  }

  /////////////////////////////////////////////////////////ART4
  if (art4_steps_count >= 0) {
    digitalWrite(dir_c, LOW);
    for (int i = 0; i < art4_steps_count; i++) {
      if (art4_steps_count == 0) {
        break;
      }
      c--;
      digitalWrite(step_c, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_c, LOW);
      delayMicroseconds(350);
    }
  }
  if (art4_steps_count < 0) {
    digitalWrite(dir_c, HIGH);
    for (int i = 0; i > art4_steps_count; i--) {
      c++;
      digitalWrite(step_c, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_c, LOW);
      delayMicroseconds(350);
    }
  }
  /////////////////////////////////////////////////////////ART5

  if (art5_steps_count >= 0) {
    digitalWrite(dir_d, LOW);
    for (int i = 0; i < art5_steps_count; i++) {
      if (art5_steps_count == 0) {
        break;
      }
      d--;
      digitalWrite(step_d, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_d, LOW);
      delayMicroseconds(350);
    }
  }
  if (art5_steps_count < 0) {
    digitalWrite(dir_d, HIGH);
    for (int i = 0; i > art5_steps_count; i--) {
      d++;
      digitalWrite(step_d, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_d, LOW);
      delayMicroseconds(350);
    }
  }
  /////////////////////////////////////////////////////////ART6

  if (art6_steps_count >= 0) {
    digitalWrite(dir_d, LOW);
    digitalWrite(dir_e, HIGH);
    for (int i = 0; i < art6_steps_count; i++) {
      if (art6_steps_count == 0) {
        break;
      }
      e--;
      digitalWrite(step_d, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_d, LOW);
      delayMicroseconds(350);
      digitalWrite(step_e, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_e, LOW);
      delayMicroseconds(350);
    }
  }
  if (art6_steps_count < 0) {
    digitalWrite(dir_d, HIGH);
    digitalWrite(dir_e, LOW);
    for (int i = 0; i > art6_steps_count; i--) {
      e++;
      digitalWrite(step_d, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_d, LOW);
      delayMicroseconds(350);
      digitalWrite(step_e, HIGH);
      delayMicroseconds(100);
      digitalWrite(step_e, LOW);
      delayMicroseconds(350);
    }
  }

  art1_steps_count = art1_steps_count + a;
  art3_steps_count = art3_steps_count + b;
  art4_steps_count = art4_steps_count + c;
  art5_steps_count = art5_steps_count + d;
  art6_steps_count = art6_steps_count + e;


  Serial.println("FINAL HOME POSITION - All of them are zero : ");
  Serial.println("1st art steps is: ");
  Serial.println(art1_steps_count);
  Serial.println("3rd art steps is: ");
  Serial.println(art3_steps_count);
  Serial.println("4th art steps is: ");
  Serial.println(art4_steps_count);
  Serial.println("5th art steps is: ");
  Serial.println(art5_steps_count);
  Serial.println("6th art steps is: ");
  Serial.println(art6_steps_count);
}

//--------------------------------------------------------------------------------------------------------------------------
//MANUAL FUNCTIONS
//manual1Motor HIGH/LOW is for the articulations 1,3,4 and 5
//manual2Motor HIGH/LOW is for the articulation 6 (rotation of the claw column)
//manualCLAW is for the CLAW - open/close
void manual1MotorHIGH(int STEP, int DIR, int amount_of_steps) {


  Serial.println("-------------BEFORE THE SUM-------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);

  Serial.println("-----------------------------------------------------------------------------");


  int manual_art1_steps_count = art1_steps_count + amount_of_steps;  //art 1 pin 28 for step
  int manual_art3_steps_count = art3_steps_count + amount_of_steps;  //art 3 pin 22 for step
  int manual_art4_steps_count = art4_steps_count + amount_of_steps;  //art 4 pin 23 for step
  int manual_art5_steps_count = art5_steps_count + amount_of_steps;  //art 5 pin 25 for step


  if (STEP == 28) {
    if (manual_art1_steps_count > 16000) {
      Serial.println("Articulation 1 cannot rotate beyond its current position");
      return;
    } else {
      art1_steps_count = art1_steps_count + amount_of_steps;
    }
  }

  if (STEP == 22) {
    if (manual_art3_steps_count > 16000) {
      Serial.println("Articulation 3 cannot rotate beyond its current position");
      return;
    } else {
      art3_steps_count = art3_steps_count + amount_of_steps;
    }
  }

  if (STEP == 23) {
    if (manual_art4_steps_count > 16000) {
      Serial.println("Articulation 4 cannot rotate beyond its current position");
      return;
    } else {
      art4_steps_count = art4_steps_count + amount_of_steps;
    }
  }

  if (STEP == 25) {
    if (manual_art5_steps_count > 16000) {
      Serial.println("Articulation 5 cannot rotate beyond its current position");
      return;
    } else {
      art5_steps_count = art5_steps_count + amount_of_steps;
    }
  }

  digitalWrite(STEP, LOW);
  digitalWrite(DIR, HIGH);

  for (int i = 0; i < amount_of_steps; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP, LOW);
    delayMicroseconds(400);
  }

  Serial.println("-------------AFTER THE SUM-------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);

  Serial.println("-----------------------------------------------------------------------------");
}

void manual1MotorLOW(int STEP, int DIR, int amount_of_steps) {



  Serial.println("-------------BEFORE THE SUM-------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);

  Serial.println("-----------------------------------------------------------------------------");


  int manual_art1_steps_count = art1_steps_count - amount_of_steps;  //art 1 pin 28 for step
  int manual_art3_steps_count = art3_steps_count - amount_of_steps;  //art 3 pin 22 for step
  int manual_art4_steps_count = art4_steps_count - amount_of_steps;  //art 4 pin 23 for step
  int manual_art5_steps_count = art5_steps_count - amount_of_steps;  //art 5 pin 25 for step


  if (STEP == 28) {
    if (manual_art1_steps_count < -16000) {
      Serial.println("Articulation 1 cannot rotate beyond its current position");
      return;
    } else {
      art1_steps_count = art1_steps_count - amount_of_steps;
    }
  }

  if (STEP == 22) {
    if (manual_art3_steps_count < -16000) {
      Serial.println("Articulation 3 cannot rotate beyond its current position");
      return;
    } else {
      art3_steps_count = art3_steps_count - amount_of_steps;
    }
  }

  if (STEP == 23) {
    if (manual_art4_steps_count < -16000) {
      Serial.println("Articulation 4 cannot rotate beyond its current position");
      return;
    } else {
      art4_steps_count = art4_steps_count - amount_of_steps;
    }
  }

  if (STEP == 25) {
    if (manual_art5_steps_count < -16000) {
      Serial.println("Articulation 5 cannot rotate beyond its current position");
      return;
    } else {
      art5_steps_count = art5_steps_count - amount_of_steps;
    }
  }

  digitalWrite(STEP, LOW);
  digitalWrite(DIR, LOW);

  for (int i = 0; i < amount_of_steps; i++) {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP, LOW);
    delayMicroseconds(400);
  }

  Serial.println("-------------AFTER THE SUM-------------------------------------------------");
  Serial.println("art1_steps_count: ");
  Serial.println(art1_steps_count);
  Serial.println("art3_steps_count: ");
  Serial.println(art3_steps_count);
  Serial.println("art4_steps_count: ");
  Serial.println(art4_steps_count);
  Serial.println("art5_steps_count: ");
  Serial.println(art5_steps_count);
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-----------------------------------------------------------------------------");
}


void manual2MotorHIGH(int STEP1, int DIR1, int STEP2, int DIR2, int amount_of_steps) {


  Serial.println("-------------BEFORE THE SUM--------------------------------------------------");
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-----------------------------------------------------------------------------");


  int manual_art6_steps_count = art6_steps_count + amount_of_steps;  //art 6 pin 25 and 27 for step


  if (manual_art6_steps_count > 7000) {
    Serial.println("The claw cannot rotate beyond its current position");
    return;
  } else {
    art6_steps_count = art6_steps_count + amount_of_steps;
  }



  digitalWrite(STEP1, LOW);
  digitalWrite(STEP2, LOW);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, LOW);


  for (int i = 0; i < amount_of_steps; i++) {
    digitalWrite(STEP1, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP1, LOW);
    delayMicroseconds(400);

    digitalWrite(STEP2, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP2, LOW);
    delayMicroseconds(400);
  }

  Serial.println("-------------AFTER THE SUM-------------------------------------------------");
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-----------------------------------------------------------------------------");
}

void manual2MotorLOW(int STEP1, int DIR1, int STEP2, int DIR2, int amount_of_steps) {


  Serial.println("-------------BEFORE THE SUM--------------------------------------------------");
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-----------------------------------------------------------------------------");


  int manual_art6_steps_count = art6_steps_count - amount_of_steps;  //art 6 pin 25 and 27 for step


  if (manual_art6_steps_count < -7000) {
    Serial.println("The claw cannot rotate beyond its current position");
    return;
  } else {
    art6_steps_count = art6_steps_count - amount_of_steps;
  }



  digitalWrite(STEP1, LOW);
  digitalWrite(STEP2, LOW);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);


  for (int i = 0; i < amount_of_steps; i++) {
    digitalWrite(STEP1, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP1, LOW);
    delayMicroseconds(400);

    digitalWrite(STEP2, HIGH);
    delayMicroseconds(100);
    digitalWrite(STEP2, LOW);
    delayMicroseconds(400);
  }

  Serial.println("-------------AFTER THE SUM-------------------------------------------------");
  Serial.println("art6_steps_count: ");
  Serial.println(art6_steps_count);
  Serial.println("-----------------------------------------------------------------------------");
}


void manualCLAW(int angle) {
  CLAW.write(angle);
  while (true) {

    auto avalCLAW = Serial.available();
    if (avalCLAW > 0) {

      auto inputCLAW = Serial.readStringUntil('\r');
      auto readCLAW = inputCLAW.length();
      while (avalCLAW > ++readCLAW)
        Serial.read();
      auto incomingByteCLAW = inputCLAW.toInt();
      if ((incomingByteCLAW > 90) || (incomingByteCLAW < 0)) {
        break;
      }
      CLAW.write(incomingByteCLAW);
      Serial.print("The claw moved to: ");
      Serial.print(incomingByteCLAW);
      Serial.println(" degrees");
    }
  }
  Serial.println("Thanks for playing with the THUNDERCLAW");
  Serial.println("Keep moving the articulations in MANUAL MODE: ");
  Serial.println("11 - Articulation 1 moves to the right ");
  Serial.println("12 - Articulation 1 moves to the left ");

  Serial.println("31 - Articulation 3 moves to the right ");
  Serial.println("32 - Articulation 3 moves to the left ");

  Serial.println("41 - Articulation 4 moves to the right ");
  Serial.println("42 - Articulation 4 moves to the left ");

  Serial.println("51 - Articulation 5 moves to the right ");
  Serial.println("52 - Articulation 5 moves to the left ");

  Serial.println("61 - Articulation 6 moves CW ");
  Serial.println("62 - Articulation 6 moves CCW ");

  Serial.println("71 - THUNDERCLAW Control!");

  Serial.println("4 - Return to MAIN MENU");
  return;
}
