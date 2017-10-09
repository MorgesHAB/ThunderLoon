/*
Original code by : https://github.com/Snootlab/lora_chisterapi
Edited by : Philippe Rochat & Lionel Isoz
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <RH_RF95.h>

RH_RF95 rf95;

int run = 1;

int Nbr_received_DATA = 0; // Global var to cound collected data types (when complete, we have the 3 types)
// Global string vars to store the data (a collection of 3 positioning elements)
char longitude[256]; 
char latitude[256];
char altitude[256];


/* Signal the end of the software */
void sigint_handler(int signal)
{
    run = 0;
}

void setup()
{ 
    wiringPiSetupGpio();

    if (!rf95.init()) 
    {
        fprintf(stderr, "Init failed\n");
        exit(1);
    }

    /* Tx Power is from +5 to +23 dbm */
    rf95.setTxPower(20);
    /* There are different configurations
     * you can find in lib/radiohead/RH_RF95.h 
     * at line 437 
     */
//    rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
//    rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);  ///< Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
//    rf95.setModemConfig(RH_RF95::Bw31_25Cr48Sf512); ///< Bw = 31.25 kHz, Cr = 4/8, Sf = 512chips/symbol, CRC on. Slow+long range
//    rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096);

        RH_RF95::ModemConfig mc;

        mc.reg_1d = RH_RF95_BW_250KHZ;
        mc.reg_1e = RH_RF95_SPREADING_FACTOR_4096CPS;
        mc.reg_26 = RH_RF95_REG_26_MODEM_CONFIG3;

        rf95.setModemRegisters(&mc);

    rf95.setFrequency(865.0); /* MHz */
}

void readMHPacket(uint8_t *buf)
{
	printf(">>Node: %u, Lenght: %u, Content: %s\n", buf[0], buf[1], &buf[2]);

  FILE *fd2; // File descriptor

  if(!(fd2 = fopen("/home/pi/lora_receiver/KML/All_received_data.txt", "a"))) // Open file descriptor to append into
    { 
      // crashing error --- would be cleaner with perror()
      printf("File opening for w ERROR !\n");
      //exit(1);
    } 
    else 
    { 
      fprintf(fd2, ">>Node: %u, Lenght: %u, Content: %s\n", buf[0], buf[1], &buf[2]); // Write the collection
      fclose(fd2);
    }
}


void loop()
{

    /* If we receive one message we show on the prompt
     * the address of the sender and the Rx power.
     */
    if( rf95.available() ) 
    {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len))
         {
	      readMHPacket(&buf[0]);
        }
    }
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigint_handler);

    setup();

    while( run )
    {
     loop();
     usleep(1);
    }

    return EXIT_SUCCESS;
}
