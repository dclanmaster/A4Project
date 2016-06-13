//2010.12.15 Ver.1.00 for OpenNI
int knVideoOpen(char *config);
int knVideoClose(void);
int knVideoCapStart(void);
int knVideoCapStop(void);
int knVideoCapNext(void);
int knVideoInqSize(int *x, int *y);
int knVideoDispOption(void);
unsigned char * knVideoGetImage(void);
unsigned short * knVideoGetDepthRaw(void);
unsigned char * knVideoGetDepthColor(void);
unsigned char * knVideoGetDepthImage(void);
int knVideoSetMotorPosition(float MotorPosition);
int knVideoSetLedMode(int LedMode);
void KinectDepthToWorld(float &x, float &y, float &z); 
