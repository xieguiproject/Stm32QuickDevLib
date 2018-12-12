#ifndef _kernel_h_
#define _kernel_h_

#include "os.h"
#include "Update.h"

#ifdef __cplusplus
extern "C"
{
#endif
void KernelMain(void);
static void TaskStart(void);
void * Task3(void *p_arg);
#ifdef __cplusplus
}
#endif


#define SYS_SOFT_VERSION "XG-FAST-IOT-2017-07-04"
#define SYS_HAD_VERSION  "GX_FAST-IOT-2017-07-04"

typedef struct tagAppInfo
{
	
	volatile float fCenterCm;
}AppInfo;




#endif


