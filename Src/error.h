/* error.h */

#ifndef _ERROR_H
#define _ERROR_H

#include "FreeRTOS.h"
#include "projdefs.h"
#include "portmacro.h"

void vErrorFatalLoop(void);
void vErrorFatal(uint16_t line, char* file, char* message);
void vErrorWarning(char* comment);

#endif /* _ERROR_H */

