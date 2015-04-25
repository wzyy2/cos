#include "GUI.h"
#include "GUI_X.h"

#include <arch/arch.h>
#include <cos/cos.h>

/*********************************************************************
*
*       Global data
*/
volatile int OS_TimeMS;

/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)

  Some timing dependent routines require a GetTime
  and delay function. Default time unit (tick), normally is
  1 ms.
  译:一些需要时间的相关函数需要用到gettime和延迟.
  默认时间单位为1ms.
*/

int GUI_X_GetTime(void) {
    return 0;
}

void GUI_X_Delay(int ms)
{
    Thread::delay(ms);
}

/*********************************************************************
*
*       GUI_X_Init()
*
* Note:
*     GUI_X_Init() is called from GUI_Init is a possibility to init
*     some hardware which needs to be up and running before the GUI.
*     If not required, leave this routine blank.
*
*     译:GUI_X_Init()是在gui_init()调用前,gui启动或者运行前准备.
*     如果不是必须的,可以把这个函数留空白.
*/

void GUI_X_Init(void)
{

}



/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  Called if WM is in idle state
*  译:视窗管理器空闲时候调用
*/

void GUI_X_ExecIdle(void) {}

/*********************************************************************
*
*      Logging: OS dependent

Note:
  Logging is used in higher debug levels only. The typical target
  build does not use logging and does therefor not require any of
  the logging routines below. For a release build without logging
  the routines below may be eliminated to save some space.
  (If the linker is not function aware and eliminates unreferenced
  functions automatically)
  译:系统日志层应用程序

*/

void GUI_X_Log     (const char *s) {
    printk("%s", s);
}
void GUI_X_Warn    (const char *s) {
    printk("%s", s);
}
void GUI_X_ErrorOut(const char *s) {
    printk("%s", s);
}
