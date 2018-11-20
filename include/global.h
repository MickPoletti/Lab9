#include <gtk/gtk.h>
#include <stdlib.h>
#include <iostream>
#define __STDC_FORMAT_MACROS


#ifndef _MY__GLOBAL__H
#define _MY__GLOBAL__H

using namespace std;


/**************************************************************
 * GUI window stuff
 **************************************************************/

typedef struct 
{
  GtkWidget *window1; //!< pointer to the main window of the GUI.
  GtkWidget *entry_sd; //!< the text field to contain the name of the serial device
  GtkWidget *label_voltage; //!< pointer to the label on which we display the voltage
  GtkWidget *label_opendevice; //!< pointer to the label that will be clicked to open a USB Comm device
  GtkWidget *label_closedevice; //!< pointer to the label that will be clicked to close a USB Comm device
  GtkWidget *label_send; //!< pointer to the label that will be clicked to send a packet of data to the Teensy
  GtkWidget *label_tx; //!< pointer to the transmission buffer display in the GUI
  GtkWidget *label_exit; //!< pointer to the label that will be clicked to exit the program interface and end the program in run-time.
  GtkWidget *entry_red; //!< pointer to the label that will be populated by an R value for red in RGB.
  GtkWidget *entry_green; //!< pointer to the label that will be populated by an G value for green in RGB.
  GtkWidget *entry_blue; //!< pointer to the label that will be populated by an B value for blue in RGB.
  GtkWidget *scale_red; //!< pointer to the value of the slider labeled red that will be populated by an R value for red in RGB.
  GtkWidget *scale_blue; //!< pointer to the value of the slider labeled blue that will be populated by an B value for blue in RGB.
  GtkWidget *scale_green; //!< pointer to the value of the slider labeled green that will be populated by an G value for green in RGB.
  
} Gui_Window_AppWidgets;

extern Gui_Window_AppWidgets *gui_app;

//this is the serial devices handle
extern int ser_dev;

//this is to gracefully shut down threads
extern int kill_all_threads;

//this variable is for communicating the voltage value string
extern char c_voltage_value[40];

//this is the mutex for the above variable
extern GMutex *mutex_to_protect_voltage_display;

//prototype of function for serial read thread
gpointer Serial_Read_Thread();



#endif
