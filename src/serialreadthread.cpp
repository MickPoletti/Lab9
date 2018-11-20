#include <glib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "global.h"

#define READ_THREAD_SLEEP_DURATION_US 100000 /**< Defines the amount of time to sleep in microseconds between threads */
#define BETWEEN_CHARACTERS_TIMEOUT_US 1000 /**< Defines the amount of time in microseconds to wait between each character */
#define MAX_VOLTAGE 3.3 /**< Defines the maximum voltage allowable by the board */
#define MAX_ADC_VALUE 1024.0 /**< Defines the max voltage allowable by the board in non VDC */
#define POT_COMMAND 'P' /**< Defines the character to be expected by the program when reading in a potentiometer value */

// define packet parameters
#define PACKET_START_BYTE  0xAA /**< Defines the start byte of a packet to be found when reading in from the Teensy */
#define PACKET_OVERHEAD_BYTES  3 /**< The amoutn of overhead for the packet. */
#define PACKET_MIN_BYTES  PACKET_OVERHEAD_BYTES + 1 /**< Minimum amount of bytes that must be used as overhead */
#define PACKET_MAX_BYTES  255 /**< The maxium amount of bytes allowable for tha packet. */

/*! 
  * \brief Checks that the packet sent in by the Teensy is valid
  * \param packetSize integer value of the size of the packet passed in to be checked
  * \param *packet Pointer to the packet itself
  * \details Validates the packets and must be used to check if any of the packets passed in can be used by the progra,.
  */
int validatePacket(unsigned int packetSize, unsigned char *packet)
{
    // check the packet size
    if(packetSize < PACKET_MIN_BYTES || packetSize > PACKET_MAX_BYTES)
    {
        return false;
    }

    // check the start byte
    if(packet[0] != PACKET_START_BYTE)
    {
        return false;
    }

    // check the length byte
    if(packet[1] != packetSize)
    {
        return false;
    }

    // compute the checksum
    char checksum = 0x00;
    for(int i = 0; i < packetSize - 1; i++)
    {
        checksum = checksum ^ packet[i];
    }

    // check to see if the computed checksum and packet checksum are equal
    if(packet[packetSize - 1] != checksum)
    {
        return false;
    }

    // all validation checks passed, the packet is valid
    return true;
}

/*! 
  * \brief Reads the serial communication in from the USB
  * \details This function is pre-existing do not modify.
  */
gpointer Serial_Read_Thread()
{
  ssize_t r_res;
  char ob[40];
  unsigned int count=0;
  static unsigned char buffer[PACKET_MAX_BYTES];
  unsigned int packetSize = PACKET_MIN_BYTES;
  double voltage_disp;
  bool ledState = false;
  const int LED_PIN = 13;
  int payload[3];

  while(!kill_all_threads)
    {
      if(ser_dev!=-1)
	{
	  r_res = read(ser_dev,ob,1);
	  //	  cout<<(int)ob[0]<<endl;
	  if(r_res==0)
	    {
	      usleep(BETWEEN_CHARACTERS_TIMEOUT_US);
	    }
	  else if(r_res<0)
	    {
	      cerr<<"Read error:"<<(int)errno<<" ("<<strerror(errno)<<")"<<endl;
	    }
	  else
	    {
	      //this means we have received a byte;
	      //the byte is in ob[0]
		if(count == 0 && ob[0] == PACKET_START_BYTE)
            {
                // this byte signals the beginning of a new packet
                buffer[count] = ob[0];
                count++;
                continue;
            }
            else if(count == 0)
            {
                // the first byte is not valid, ignore it and continue
                continue;
            }
            else if(count == 1)
            {
                // this byte contains the overall packet length
                buffer[count] = ob[1];

                // reset the count if the packet length is not in range
                if(packetSize < PACKET_MIN_BYTES || packetSize > PACKET_MAX_BYTES)
                {
                    count = 0;
                }
                else
                {
                    packetSize = ob[1];
                    count++;
                }
                continue;
            }
            else if(count < packetSize)
            {
                // store the byte
                buffer[count] = ob[1];
                count++;
            }

            // check to see if we have acquired enough bytes for a full packet
            if(count >= packetSize)
            {
                // validate the packet
                if(validatePacket(packetSize, buffer))
                {
//			c_voltage_value[0] = '5';
                    // change the LED state if the packet is valid
                    //ledState = !ledState;
                    //digitalWrite(LED_PIN, ledState);
                    if (buffer[2] == 'P') {
                         ob[0] = 'P';
                         ob[1] = buffer[3] >> 8;
                         ob[2] = buffer[4] & 255; 
//                         c_voltage_value = ob;
                         c_voltage_value[0] = (int)255;
                    	 c_voltage_value[1] = (int)255;
                         c_voltage_value[2] = (int)255; 
                    // echo back the received packet payload
                   // sendPacket(packetSize - PACKET_OVERHEAD_BYTES, buffer + 2);
                }

                // reset the count
                count = 0;
            }
	      //TODO: receive an entire packet and validate it.

	      
	      //TODO: Once validated, check if the third byte is a 'P'
	      //decode the payload into a single int between 0 and MAX_ADC_VALUE
	      //convert it into a voltage 0->0V  MAX_ADC_VALUE->MAX_VOLTAGE
	      //load that converted value into voltage_disp
	      //the next three lines need to be there for the value to be
	      //displayed (once you loaded voltage_value with the received
	      //voltage value

	      //once you have voltage_disp calculated us the following three
	      //lines of code to change the global variable c_voltage_value
	      //this is the variable that the main thread periodically displays
	      //in the GUI
	      g_mutex_lock(mutex_to_protect_voltage_display);
	      sprintf(c_voltage_value,"%1.3f",voltage_disp);
	      g_mutex_unlock(mutex_to_protect_voltage_display);
	
	    }
	}
}
      else { }
	usleep(READ_THREAD_SLEEP_DURATION_US);
    }

  return NULL;
}
