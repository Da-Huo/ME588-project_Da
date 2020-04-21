#include <AFMotor.h>

#define FLEFT           1 //front left wheel
#define FRIGHT          0 //front right wheel
#define BLEFT           3 //back left wheel 
#define BRIGHT          2 //back right wheel

void setMotorSpeed(int i, int spd);
void stopMotor(int i);
void setMotorSpeeds(int fleftSpeed, int frightSpeed, int bleftSpeed, int brightSpeed);
void stopMotors();
