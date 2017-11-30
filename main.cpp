/*
Original code by : https://github.com/Snootlab/lora_chisterapi
Edited by : Philippe Rochat & Lionel Isoz
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <RH_RF95.h>

RH_RF95 rf95;

#define GPSTIME 0
#define SPEED 1
#define LONGITUDE 2
#define LATITUDE 3
#define ALTITUDE 4

#define NODE_NUMBER 8  //Max : 255

//string NODE_NUMBER;
//NODE_NUMBER = "MorgesHAB";

uint8_t msg[250];  //max : 255

int run = 1;

/* Send a message every 3 seconds */
void sigalarm_handler(int signal)
{
   rf95.send(msg, sizeof(msg));
   rf95.waitPacketSent();
//   printf("Sent!\n");
   alarm(3);
}

void File(char *name) {
    FILE* fichier = NULL;
    int caractereActuel = 0;
//    printf("\n%s",name);

    fichier = fopen(name, "rb");

    if (fichier != NULL)
    {
        // Boucle de lecture des caractères un à un
        do
        {
           caractereActuel = fgetc(fichier); // On lit le caractère
          // printf("%c", caractereActuel); // On l'affiche
// 	    msg[2]+= caractereActuel; // We should use strncpy !!!!

	    FILE *fd3; // File descriptor

  if(!(fd3 = fopen("/home/pi/photo/new.JPG", "ab"))) // Open file descriptor to append into
    { 
      printf("File opening for w ERROR !\n");
    } 
    else 
    { 
      fprintf(fd3, "%c", caractereActuel); // Write the collection
      fclose(fd3);
    }

      } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)

        fclose(fichier);
    }
  
}


/* Compose a message to be sent in a MorgesHabPacket : MHPacket */
/*Syntax is:
  byte 0: node number
  byte 1: msg type
  byte 2: msg length (max 200-4)
  byte 3 & ... : ASCII encoded value, null terminated
*/
void writeMHPacket(char *m) {

  msg[0] = NODE_NUMBER;
  msg[1] = (uint8_t)strlen(m);
//  strcpy((char *)&msg[2], m); // We should use strncpy !!!!

int k = 0;
int i = 0;
char str_i[10];

for (i = 0; i < 207; ++i)
{
  if(i < 10) {
    char filename[30] = "/home/pi/photo/p0";
    sprintf(str_i, "%d", i);
    strcat(filename, str_i);
    printf("\n%s",filename);
    File(filename);
  }
  if(i > 9 && i < 90) {
    char filename[30] = "/home/pi/photo/p";
    sprintf(str_i, "%d", i);
    strcat(filename, str_i);
    printf("\n%s",filename);
    File(filename);
  }
  if(i > 89) {
    char filename[30] = "/home/pi/photo/p";
    k = i + 8910;
    sprintf(str_i, "%d", k);
    strcat(filename, str_i);
    printf("\n%s",filename);
    File(filename);
  }
 }
}


/* get content from such a packet ... */

void readMHPacket(uint8_t *msg) {
  printf(">>Node: %u, Length: %u, Content: %s\n", msg[0], msg[1], &msg[2]);

FILE *fd2; // File descriptor

  if(!(fd2 = fopen("/home/pi/photo/picture.JPG", "a"))) // Open file descriptor to append into
    { 
      // crashing error --- would be cleaner with perror()
      printf("File opening for w ERROR !\n");
      //exit(1);
    } 
    else 
    { 
      fprintf(fd2, "\n%s", &msg[2]); // Write the collection
      fclose(fd2);
    }
}


/* Signal the end of the software */
void sigint_handler(int signal)
{
    run = 0;
}

void setup() {
     wiringPiSetupGpio();

     if (!rf95.init()) 
     {
         fprintf(stderr, "Init failed\n");
         exit(1);
     }

     /* Tx power is from +5 to +23 dBm */
     rf95.setTxPower(20);
     /* There are different configurations
      * you can find in lib/radiohead/RH_RF95.h 
      * at line 437 
      */
//     rf95.setModemConfig(RH_RF95::Bw125Cr45Sf128); //Default --> Medium Range
//     rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128); ///< Bw = 500 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Fast+short range
//     rf95.setModemConfig(RH_RF95::Bw31_25Cr48Sf512); //Slow + Long Range
//     rf95.setModemConfig(RH_RF95::Bw125Cr48Sf4096); //Slow + Long Range-->Spreadind factor 12


        RH_RF95::ModemConfig mc;

        mc.reg_1d = RH_RF95_BW_500KHZ;
        mc.reg_1e = RH_RF95_SPREADING_FACTOR_128CPS;
        mc.reg_26 = RH_RF95_REG_26_MODEM_CONFIG3;

        rf95.setModemRegisters(&mc);



     rf95.setFrequency(865.0); /* Mhz */
}

int main(int argc, char ** argv)
{
    signal(SIGINT, sigint_handler);
    signal(SIGALRM, sigalarm_handler);

    alarm(3);

    setup();
//    printf(argv[1],"\n");
  // i will run through GPSTIME, SPEED, LONGITUDE, LATITUDE, ALTITUDE
  for(int i = 1; i<argc; i++) { // Skip 0 which is program itself
    writeMHPacket(argv[i]);
    sigalarm_handler(1); // Fake a signal
    readMHPacket(&msg[0]);
  }

    return EXIT_SUCCESS;
}
