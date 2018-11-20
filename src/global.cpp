#include "global.h"
/*! 
  * \brief Structure to keep all interesting widgets
  */
Gui_Window_AppWidgets *gui_app; 

int ser_dev=-1; //!< pointer to the value of the slider labeled red that will be populated by an R value for red in RGB.

char c_voltage_value[40]; //!< Character array for the voltage value read in from the potentiometer.

int kill_all_threads; //!< Integer intended to be set when the program needs to end. Thereby ending all threads

GMutex *mutex_to_protect_voltage_display; //!< Pointer of type GMutex used to protect the voltage display value.



  

