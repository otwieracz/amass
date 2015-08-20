/* error.h */

#ifndef _ERROR_H
#define _ERROR_H

#include "projdefs.h"
#include "portmacro.h"

void vErrorFatalLoop(void);
void vErrorFatal(char* comment);
void vErrorWarning(char* comment);

#endif /* _ERROR_H */

