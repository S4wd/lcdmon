#ifndef SIMPSON4WD_TEMP_H
#define SIMPSON4WD_TEMP_H

//#ifdef __cplusplus
//extern "C" {
//#endif

int s4d_OWAcquireBusController(char *);
char *s4d_OWFindSensor(int);     
float s4d_OWReadTemperature(int BusId, char *);
void s4d_OWReleaseBusController(int);

//#ifdef __cplusplus
//}
//#endif

#endif
