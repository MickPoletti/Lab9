/*!
  * \author Mason McDaniel
  * \version 1.0
  * \date 2018-11-20
  * \bug Whicn clicking on the x in the corner the application does not terminate correctly.
  *
  * \warning improper use of this code may void you car's warranty
  *
  * \copyright GNU Public License
  *
  * \mainpage The Teensy LED Controller
  *  \section intro_sec Introduction
  * This code was developed to torture students.
  * \section compile_sec Compilation
  * Here I would describe how to compile this code with cmake and make
  * \subsection Step1 Cmake
  * How to use cmake
  * \subsection Step2 make
  * How to use make
  */ 

#include "global.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#define VOLTAGE_DISPLAY_UPDATE_MS 100 /**< Sets the rate at which the voltage display will be updated (by default 100 ms) in milliseconds */

/*! 
  * \brief Connects the widget pointers to the glade widgets
  * \param p_builder pointer to the builder structure
  * \details For this function to work, the widget class must have pointers for all widgets that we want to 
  * modify from the code.
  */
  
void ObtainGuiWidgets(GtkBuilder *p_builder)
{
  #define GuiappGET(xx) gui_app->xx=GTK_WIDGET(gtk_builder_get_object(p_builder,#xx)) /**< Defines the function GuiappGET to be used to find each widget defined in glade. */
  GuiappGET(window1);
  GuiappGET(entry_sd);
  GuiappGET(label_voltage); 
  GuiappGET(label_opendevice);
  GuiappGET(label_closedevice);
  GuiappGET(label_send);
  GuiappGET(label_tx);
  GuiappGET(label_exit);
  GuiappGET(entry_red);
  GuiappGET(entry_green);
  GuiappGET(entry_blue);
  GuiappGET(scale_red);
  GuiappGET(scale_blue);
  GuiappGET(scale_green);	
    
}


/*! 
  * \brief Displays voltage to be inputted by the Teensy.
  * \param p_gptr Pointer to the data where voltage is stored
  * \details This function was included with the program do not alter its code!
  */
gboolean  Voltage_Display_Displayer(gpointer p_gptr)
{
  // do not change this function
  g_mutex_lock(mutex_to_protect_voltage_display);
  gtk_label_set_text(GTK_LABEL(gui_app->label_voltage),c_voltage_value);
  g_mutex_unlock(mutex_to_protect_voltage_display);
  return true;
}

/*! 
  * \brief Detects when the button labeled "Open Device" is clicked
  * \param p_wdgt Pointer to the widget that button_opendevice_clicked will be checking 
  * \param p_data Pointer to the data in the widget button_open_device_clicked will be looking for
  * \details This function was included with the program do not alter its code!
  */
extern "C" void button_opendevice_clicked(GtkWidget *p_wdgt, gpointer p_data ) 
{
  //do not change  the next few lines
  //they contain the mambo-jumbo to open a serial port
  
  const char *t_device_value;
  struct termios my_serial;

  t_device_value = gtk_entry_get_text(GTK_ENTRY(gui_app->entry_sd));
  //open serial port with read and write, no controling terminal (we don't
  //want to get killed if serial sends CTRL-C), non-blocking 
  ser_dev = open(t_device_value, O_RDWR | O_NOCTTY );
  
  bzero(&my_serial, sizeof(my_serial)); // clear struct for new port settings 
        
  //B9600: set baud rate to 9600
  //   CS8     : 8n1 (8bit,no parity,1 stopbit)
  //   CLOCAL  : local connection, no modem contol
  //   CREAD   : enable receiving characters  */
  my_serial.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
         
  tcflush(ser_dev, TCIFLUSH);
  tcsetattr(ser_dev,TCSANOW,&my_serial);

  //You can add code beyond this line but do not change anything above this line
  
}


/*! 
  * \brief Detects when the button labeled "Close Device" is clicked
  * \param p_wdgt Pointer to the widget that button_close_clicked will be checking 
  * \param p_data Pointer to the data in the widget button_open_device_clicked will be looking for
  * \details This function was included with the program do not alter its code!
  */
extern "C" void button_closedevice_clicked(GtkWidget *p_wdgt, gpointer p_data ) 
{
  //this is how you disable a button:
  gtk_widget_set_sensitive (gui_app->label_closedevice,FALSE);
  //this is how you enable a button:
  gtk_widget_set_sensitive (gui_app->label_opendevice,TRUE);

  //do not change the next two lines; they close the serial port
  close(ser_dev);
  ser_dev=-1;

}


/*! 
  * \brief Detects when the button labeled "Send" is clicked
  * \param p_wdgt Pointer to the widget that button_send_clicked will be checking 
  * \param p_data Pointer to the data in the widget button_open_device_clicked will be looking for
  * \details This function gets the valeus entered in the labels and status of the sliders and then sends a string to the Teensy to be displayed as an RGB value.
  */
extern "C" void button_send_clicked(GtkWidget *p_wdgt, gpointer p_data ) 
{
  const char *t_red_value;
  const char *t_blue_value;
  const char *t_green_value;
  int i_red_value;
  int i_blue_value;
  int i_green_value;
  unsigned char uc_red_value;
  unsigned char uc_blue_value;
  unsigned char uc_green_value;
  char r_cc_value[40];
  char b_cc_value[40];
  char g_cc_value[40];
  char send_buff[7];
  int length_send_buff = 7;


  //getting text from widget:
  t_red_value = gtk_entry_get_text(GTK_ENTRY(gui_app->entry_red));
  t_blue_value = gtk_entry_get_text(GTK_ENTRY(gui_app->entry_blue));
  t_green_value = gtk_entry_get_text(GTK_ENTRY(gui_app->entry_green));
  
  uc_red_value = atoi(t_red_value);
  uc_blue_value = atoi(t_blue_value);
 uc_green_value = atoi(t_green_value);
  

  //setting range on scale slider to uc_red_value 
  /*gtk_range_set_value(GTK_RANGE(gui_app->scale_red),uc_red_value);
  gtk_range_set_value(GTK_RANGE(gui_app->scale_red),uc_blue_value);
  gtk_range_set_value(GTK_RANGE(gui_app->scale_red),uc_green_value);	
	*/	      
  //setting text on label
 
  //sprintf(r_cc_value,"%d",uc_red_value);
  //sprintf(b_cc_value,"%d",uc_blue_value);
  //sprintf(g_cc_value,"%d",uc_green_value);
   gtk_label_set_text(GTK_LABEL(gui_app->label_tx),r_cc_value);
   gtk_range_set_value(GTK_RANGE(gui_app->scale_red),uc_red_value);
   gtk_range_set_value(GTK_RANGE(gui_app->scale_blue),uc_blue_value);
   gtk_range_set_value(GTK_RANGE(gui_app->scale_green),uc_green_value);
  send_buff[0] = 0xAA;
  send_buff[1] = 7;	
  send_buff[2] = 'L';
  send_buff[3] = uc_red_value;
  send_buff[4] = uc_blue_value;
  send_buff[5] = uc_green_value;
  send_buff[6] = send_buff[0]^send_buff[1]^send_buff[2]^send_buff[3]^send_buff[4]^send_buff[5];

  sprintf(r_cc_value,"%X %X %X %X %X %X %X",send_buff[0], send_buff[1], send_buff[2], send_buff[3], send_buff[4], send_buff[5], send_buff[6]);
  //this is how you send an array out on the serial port:
  write(ser_dev,send_buff,length_send_buff);
}


/*! 
  * \brief Detects when one of the sliders values have changed.
  * \param p_wdgt Pointer to the widget that scale_rgb_value_changed will be checking 
  * \param p_data Pointer to the data in the widget scale_rgb_value_changed will be looking for
  * \details This function needs to convert from char values to ints and then back into char data 
   * and still send a signal in bit form to the Teensy.
  */
extern "C" void scale_rgb_value_changed(GtkWidget *p_wdgt, gpointer p_data ) 
{
   unsigned char uc_red_value;
   unsigned char uc_blue_value;
   unsigned char uc_green_value;
   char c_cc_value[40];
   char cc_red_value[40];
   char cc_blue_value[40];
   char cc_green_value[40];
   char send_buff[7];
  //getting the value of the scale slider 
   double g_red_value = gtk_range_get_value(GTK_RANGE(gui_app->scale_red));
   double g_blue_value = gtk_range_get_value(GTK_RANGE(gui_app->scale_blue));
   double g_green_value = gtk_range_get_value(GTK_RANGE(gui_app->scale_green));
   sprintf(cc_red_value,"%g",g_red_value); 
   sprintf(cc_green_value,"%g",g_green_value); 
   sprintf(cc_blue_value,"%g",g_blue_value); 



    


  //setting text on entry
 /* sprintf(c_cc_value,"%d",uc_red_value);
  sprintf(c_cc_value,"%d",uc_blue_value);
  sprintf(c_cc_value,"%d",uc_green_value);
*/
  gtk_entry_set_text(GTK_ENTRY(gui_app->entry_red),cc_red_value);
  gtk_entry_set_text(GTK_ENTRY(gui_app->entry_blue),cc_blue_value);
  gtk_entry_set_text(GTK_ENTRY(gui_app->entry_green),cc_green_value);

  send_buff[0] = 0xAA;
  send_buff[1] = 7;	
  send_buff[2] = 'L';
  send_buff[3] = g_red_value;
  send_buff[4] = g_blue_value;
  send_buff[5] = g_green_value;
  send_buff[6] = send_buff[0]^send_buff[1]^send_buff[2]^send_buff[3]^send_buff[4]^send_buff[5];

   sprintf(c_cc_value,"%X %X %X %X %X %X %X",send_buff[0], send_buff[1], send_buff[2], send_buff[3], send_buff[4], send_buff[5], send_buff[6]);
   gtk_label_set_text(GTK_LABEL(gui_app->label_tx),c_cc_value);
   write(ser_dev,send_buff,7);
}

/*! 
  * \brief Detects when the button labeled "Exit" is clicked
  * \param p_wdgt Pointer to the widget that button_exit_clicked will be checking 
  * \param p_data Pointer to the data in the widget button_exit_device_clicked will be looking for
  * \details This function simply ends the program when the button labeled "Exit" is clicked.
  */
extern "C" void button_exit_clicked(GtkWidget *p_wdgt, gpointer p_data ) 
{
  gtk_main_quit();

}



/*! 
  * \brief 
  * \param argc Takes in the amount of paramters as an int passed in by the program when run.
  * \param **argv Pointer to the individual parameters run with command line arguments.
  * \details This function was included with the program do not alter its code
  */
int main(int argc, char **argv)
{

  GtkBuilder *builder;
  GError *err = NULL;

  GThread *read_thread;

  //this is how you allocate a Glib mutex
  g_assert(mutex_to_protect_voltage_display == NULL);
  mutex_to_protect_voltage_display = new GMutex;
  g_mutex_init(mutex_to_protect_voltage_display);

  // this is used to signal all threads to exit
  kill_all_threads=false;
  
  //spawn the serial read thread
  read_thread = g_thread_new(NULL,(GThreadFunc)Serial_Read_Thread,NULL);
  
  // Now we initialize GTK+ 
  gtk_init(&argc, &argv);
  
  //create gtk_instance for visualization
  gui_app = g_slice_new(Gui_Window_AppWidgets);

  //builder
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "teensy_control.glade", &err);

  
  //error handling
  if(err)
    {
      g_error(err->message);
      g_error_free(err);
      g_slice_free(Gui_Window_AppWidgets, gui_app);
      exit(-1);
    }

  // Obtain widgets that we need
  ObtainGuiWidgets(builder);

  // Connect signals
  gtk_builder_connect_signals(builder, gui_app);

  // Destroy builder now that we created the infrastructure
  g_object_unref(G_OBJECT(builder));

  //display the gui
  gtk_widget_show(GTK_WIDGET(gui_app->window1));

  //this is going to call the Voltage_Display_Displayer function periodically
  gdk_threads_add_timeout(VOLTAGE_DISPLAY_UPDATE_MS,Voltage_Display_Displayer,NULL);

  //the main loop
  gtk_main();

  //signal all threads to die and wait for them (only one child thread)
  kill_all_threads=true;
  g_thread_join(read_thread);
  
  //destroy gui if it still exists
  if(gui_app)
    g_slice_free(Gui_Window_AppWidgets, gui_app);

  return 0;
}
