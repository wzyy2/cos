/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              �C/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : WIDGET_SimpleProgbar.c
Purpose     : Demonstrates the use of the PROGBAR widget
----------------------------------------------------------------------
*/

#include "GUI.h"
#include "PROGBAR.h"

/*******************************************************************
*
*       static code
*
********************************************************************
*/

/*******************************************************************
*
*        _DemoProgBar
*/
static void _DemoProgBar(void) {
  PROGBAR_Handle ahProgBar;
  int i;
  GUI_SetFont(&GUI_Font8x16);
  GUI_DispStringAt("Progress bar", 100,80);
  /* Create progress bar */  
  ahProgBar = PROGBAR_Create(100, 100, 100, 20, WM_CF_SHOW);
  GUI_Delay (500);
  /* Modify progress bar */
  for (i = 0; i <= 100; i++) {
    PROGBAR_SetValue(ahProgBar, i);
    GUI_Delay(10);
  }
  GUI_Delay (400);
  /* Delete progress bar */  
  PROGBAR_Delete(ahProgBar);
  GUI_ClearRect(0, 50, 319, 239);
  GUI_Delay(750);
}

/*******************************************************************
*
*       MainTask
*
*       Shows the use of progress bars
*
********************************************************************
*/

void MainTask(void) {
  GUI_Init();
  GUI_SetBkColor(GUI_BLACK);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringHCenterAt("WIDGET_SimpleProgbar - Sample", 160, 5);
  while(1) {
    _DemoProgBar();
  }
}
