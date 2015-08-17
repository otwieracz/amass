/* error.h */

#include "hardware.h"

void vErrorFatalLoop(void);
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line);
#endif

