#include <Servo.h>

double x = 0;  
double y = 0;  
double z = 198; 
double psi = 0;               
double theta = 0;          
double phi = 0;              

double M[3][3];
double rxp[3][6];
double L[3][6];

double Len[6];

int pulse[6];

Servo S[6];

int pins[6] = {3, 5, 6, 9, 10, 11};  

double EEAx =  cos(PI/12) * 66;
double EEAy = -sin(PI/12) * 66;

double EEBx =  cos(PI/12) * 66;
double EEBy =  sin(PI/12) * 66;

double EECx = -sin(PI/12) * 66;
double EECy =  cos(PI/12) * 66;

double EEDx = -sin(PI/4 ) * 66;
double EEDy =  cos(PI/4 ) * 66;

double EEEx = -sin(PI/4 ) * 66;
double EEEy = -cos(PI/4 ) * 66;

double EEFx = -sin(PI/12) * 66;
double EEFy = -cos(PI/12) * 66;

double EEz = 0;//cos(30) * 180;

double BBAx =  cos(3*PI/12) * 66;
double BBAy = -sin(3*PI/12) * 66;

double BBBx =  cos(3*PI/12) * 66;
double BBBy =  sin(3*PI/12) * 66;

double BBCx =  sin(PI/12) * 66;
double BBCy =  cos(PI/12) * 66;

double BBDx = -cos(PI/12 ) * 66;
double BBDy =  sin(PI/12 ) * 66;

double BBEx = -cos(PI/12 ) * 66;
double BBEy = -sin(PI/12 ) * 66;

double BBFx =  sin(PI/12) * 66;
double BBFy = -cos(PI/12) * 66;

double BBz = 0;


void setup(){
  
  Serial.begin(9600);
 // Serial.println("test");
  for (int i = 0; i < 6; i++){
    S[i].attach(pins[i]);
  }

  moveplatform(0,0,10,0,0,0);
}

double angle = 0;
char anglec = 0;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void serialEvent() {
  while (Serial.available()) {
    // get the new byte: matrix only sends one byte angle data at one time
    anglec = (char)Serial.read();
    stringComplete = true;
  }
}



void loop()
{

    if(stringComplete){
      // The Serial.parseInt() parses the input bytes in "string" format. For example, if the input value is 67, it will be parsed as number "7".
      // To get angle 45 degrees, the input should be two bytes with value of 64 and 65 respectively.
      //angle = (double)Serial.parseInt();
      // The received angle data is with one byte int8_t format. It's a "binary" value instead of "string". For example if the input value is 45,
      // it represents the actual angle 45 degrees.
      angle = (double)anglec;
      moveplatform(0,0,10,0,0,angle);
      //Serial.println(angle);

      stringComplete = false;
    
      //delay(100);
    }

/*
  if(angle<45)
    angle+=1;
  else
    angle=-45;  

 // time = -PI/2;  

 //time = 1;   
 // while (Serial.available()>0)
 //   psi=rad(Serial.parseFloat()); 
    
//  moveplatform(50*sin(time),-50*cos(time),5,20*sin(time),20*cos(time),0);
//  moveplatform(50*sin(time),-50*cos(time),5,0,0,0);
   moveplatform(0,0,5,0,0,angle);

   Serial.println(angle);
   

  delay(500);
  */
}

double rad(double a){
  return a*PI/180;
}

void moveplatform(float xin, float yin, float zin, float roll, float pitch,float yaw){
  if(xin<-60) xin = -60; 
  if(xin>60) xin = 60; 
    x = xin;

  if(yin<-60) yin = -60; 
  if(yin>60) yin = 60; 
    y = yin;

  if(zin<-30) zin = -30; 
  if(zin>30) zin = 30; 
    z = zin + 198;    

  if(roll<-30) roll = -30; 
  if(roll>30) roll = 30; 
    theta = rad(roll);

  if(pitch<-20) pitch = -20; 
  if(pitch>20) pitch = 20; 
    phi = rad(pitch);  

  if(yaw<-40) yaw = -40; 
  if(yaw>40) yaw = 40; 
    psi = rad(yaw); 

  getmatrix();
  getrxp();
  getL();
  getLen();
  getPulses();
  runservos();
   
}

void getmatrix(){
  
  M[0][0] = cos(psi)*cos(theta);
  M[0][1] = -sin(psi)*cos(phi)+cos(psi)*sin(theta)*sin(phi);
  M[0][2] = sin(psi)*sin(phi)+cos(psi)*cos(phi)*sin(theta);
  M[1][0] = sin(psi)*cos(theta);
  M[1][1] = cos(psi)*cos(phi)+sin(psi)*sin(theta)*sin(phi);
  M[1][2] = -cos(psi)*sin(phi)+sin(psi)*sin(theta)*cos(phi);
  M[2][0] = -sin(theta);
  M[2][1] = cos(theta)*sin(phi);
  M[2][2] = cos(theta)*cos(phi);
  
}

void getrxp(){
  
  rxp[0][0] = M[0][0]*(EEAx)+M[0][1]*(EEAy)+M[0][2]*EEz;
  rxp[1][0] = M[1][0]*(EEAx)+M[1][1]*(EEAy)+M[1][2]*EEz;
  rxp[2][0] = M[2][0]*(EEAx)+M[2][1]*(EEAy)+M[2][2]*EEz;
  
  rxp[0][1] = M[0][0]*(EEBx)+M[0][1]*(EEBy)+M[0][2]*EEz;
  rxp[1][1] = M[1][0]*(EEBx)+M[1][1]*(EEBy)+M[1][2]*EEz;
  rxp[2][1] = M[2][0]*(EEBx)+M[2][1]*(EEBy)+M[2][2]*EEz;
  
  rxp[0][2] = M[0][0]*(EECx)+M[0][1]*(EECy)+M[0][2]*EEz;
  rxp[1][2] = M[1][0]*(EECx)+M[1][1]*(EECy)+M[1][2]*EEz;
  rxp[2][2] = M[2][0]*(EECx)+M[2][1]*(EECy)+M[2][2]*EEz;
  
  rxp[0][3] = M[0][0]*(EEDx)+M[0][1]*(EEDy)+M[0][2]*EEz;
  rxp[1][3] = M[1][0]*(EEDx)+M[1][1]*(EEDy)+M[1][2]*EEz;
  rxp[2][3] = M[2][0]*(EEDx)+M[2][1]*(EEDy)+M[2][2]*EEz;
  
  rxp[0][4] = M[0][0]*(EEEx)+M[0][1]*(EEEy)+M[0][2]*EEz;
  rxp[1][4] = M[1][0]*(EEEx)+M[1][1]*(EEEy)+M[1][2]*EEz;
  rxp[2][4] = M[2][0]*(EEEx)+M[2][1]*(EEEy)+M[2][2]*EEz;
  
  rxp[0][5] = M[0][0]*(EEFx)+M[0][1]*(EEFy)+M[0][2]*EEz;
  rxp[1][5] = M[1][0]*(EEFx)+M[1][1]*(EEFy)+M[1][2]*EEz;
  rxp[2][5] = M[2][0]*(EEFx)+M[2][1]*(EEFy)+M[2][2]*EEz;
  
}


void getL(){
  L[0][0] = rxp[0][0]-(BBAx)+x;
  L[1][0] = rxp[1][0]-(BBAy)+y;
  L[2][0] = rxp[2][0]-(BBz)+z;
  
  L[0][1] = rxp[0][1]-(BBBx)+x;
  L[1][1] = rxp[1][1]-(BBBy)+y;
  L[2][1] = rxp[2][1]-(BBz)+z;
  
  L[0][2] = rxp[0][2]-(BBCx)+x;
  L[1][2] = rxp[1][2]-(BBCy)+y;
  L[2][2] = rxp[2][2]-(BBz)+z;
  
  L[0][3] = rxp[0][3]-(BBDx)+x;
  L[1][3] = rxp[1][3]-(BBDy)+y;
  L[2][3] = rxp[2][3]-(BBz)+z;
  
  L[0][4] = rxp[0][4]-(BBEx)+x;
  L[1][4] = rxp[1][4]-(BBEy)+y;
  L[2][4] = rxp[2][4]-(BBz)+z;
  
  L[0][5] = rxp[0][5]-(BBFx)+x;
  L[1][5] = rxp[1][5]-(BBFy)+y;
  L[2][5] = rxp[2][5]-(BBz)+z;

}

void getLen(){
  for (int i = 0; i < 6; i++){
    Len[i] = sqrt(L[0][i]*L[0][i]+L[1][i]*L[1][i]+L[2][i]*L[2][i]); //changed to array
    if(Len[i]>225) Len[i] = 225;
    if(Len[i]<180) Len[i] = 180;
  } 
}

void getPulses(){
  for (int i = 0; i < 6; i++){
    pulse[i] = (int) ((Len[i]-180)*(160-40)/(225-180))+40;
  }
}

void runservos()
{
  for (int i = 0; i < 6; i++){
    S[i].write(pulse[i]);// changed S to an array            
  }
}
