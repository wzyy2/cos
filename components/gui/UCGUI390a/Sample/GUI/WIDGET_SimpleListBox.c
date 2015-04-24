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
File        : WIDGET_SimpleListBox.c
Purpose     : Example demonstrating the LISTBOX widget
----------------------------------------------------------------------
*/

#include "GUI.h"
#include "LISTBOX.h"
#include <stddef.h>

/*******************************************************************
*
*       defines
*
********************************************************************
*/

#define countof(Array) (sizeof(Array) / sizeof(Array[0]))

/*******************************************************************
*
*       static variables
*
********************************************************************
*/

static const GUI_ConstString _ListBox[] = {
  "English", "Deutsch", "Fran�ais", "Japanese", "Italiano", NULL
};

/*******************************************************************
*
*       static code
*
********************************************************************
*/

/*******************************************************************
*
*       _DemoListBox
*/
void _DemoListBox(void) {
  int i;
  int Entries = countof(_ListBox) - 1;
  LISTBOX_Handle hListBox;
  int ySize = GUI_GetYDistOfFont(&GUI_Font13B_1) * Entries;
  /* Create the listbox */
  hListBox = LISTBOX_Create(_ListBox, 130, 80, 60, ySize, WM_CF_SHOW);
  /* Change current selection of the listbox */
  for (i = 0; i < Entries-1; i++) {
    GUI_Delay(500);
    LISTBOX_IncSel(hListBox);
    WM_ExecIdle();
	}
  for (i = 0; i < Entries-1; i++) {
    GUI_Delay(500);
    LISTBOX_DecSel(hListBox);
    WM_ExecIdle();
	}
  GUI_Delay(750);
  /* Delete listbox widget */
  LISTBOX_Delete(hListBox);
  GUI_ClearRect(0, 50, 319, 239);
  GUI_Delay(750);
}

/*******************************************************************
*
*       MainTask
*
*       Demonstrates LISTBOX widget
*
********************************************************************
*/

void MainTask(void) {
  GUI_Init();
  GUI_SetBkColor(GUI_BLUE);
  GUI_Clear();
  GUI_SetColor(GUI_WHITE);
  GUI_SetFont(&GUI_Font24_ASCII);
  GUI_DispStringHCenterAt("WIDGET_SimpleListBox - Sample", 160, 5);
  while(1) {
    _DemoListBox();
  }
}
