/* 
 * RF gw for JABK Home Automation
 *
 */

#include <cstdlib>
#include <iostream>
#include <RF24/RF24.h>

using namespace std;

char receivePayload[32];

// Radio pipe addresses for the 2 nodes to communicate.
// First pipe is for writing, 2nd, 3rd, 4th, 5th & 6th is for reading...
const uint64_t pipes[6] = 
					{ 0xF0F0F0F0D2LL, 0xF0F0F0F0E1LL, 
					  0xF0F0F0F0E2LL, 0xF0F0F0F0E3LL, 
					  0xF0F0F0F0F1, 0xF0F0F0F0F2 
					};

// CE Pin, CSN Pin, SPI Speed
// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ); 


//#include <sqlite3.h>
#include <stdio.h>
#include <time.h>


typedef struct day_t
{
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint8_t year;
} Today;

struct Payload
{
	char nodeID;
	char counter;
	char openOrClosed;
	char receiveEnabled;
	int16_t  temperature;
	int16_t  batteryVoltage;
	long time;
	Today openTime;
	Today closeTime;
} __attribute__ ((packed));

// sqlite3 *db;

// int initDB() {
//   int rc;
  
//   rc = sqlite3_open("/tmp/sensors.db", &db);
//   if( rc ){
//     fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//     sqlite3_close(db);
//     return(1);
//   }
// }

// int closeDB() {
//   sqlite3_close(db);
//   return 0;
// }


struct HomeMonitorProtocol_v1 {
  unsigned char id;
  unsigned int temp;
  unsigned int batt;
};

int main(int argc, char** argv) 
{
	uint8_t len;

	// initDB();
	FILE *fd;
	if (argc == 1)
	  fd = fopen("/tmp/el.csv","a");

	// Refer to RF24.h or nRF24L01 DS for settings
	radio.begin();
	// Enable this seems to work better
	radio.enableDynamicPayloads();
	radio.setAutoAck(1);
	radio.setDataRate(RF24_250KBPS);
	radio.setPALevel(RF24_PA_MAX);
	radio.setChannel(70);
	radio.setRetries(15,15);
	radio.setCRCLength(RF24_CRC_8);

	// Open 6 pipes for readings ( 5 plus pipe0, also can be used for reading )
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1,pipes[1]);
	radio.openReadingPipe(2,pipes[2]);
	radio.openReadingPipe(3,pipes[3]);
	radio.openReadingPipe(4,pipes[4]);
	radio.openReadingPipe(5,pipes[5]);

	//
	// Start listening
	//
	radio.startListening();

	//
	// Dump the configuration of the rf unit for debugging
	//
	radio.printDetails();
	
	printf("Output below : \n");
	delay(1);

	time_t timeInSeconds;
	while(1)
	{
		char receivePayload[32];
		uint8_t pipe = 1;
		
		// Start listening
		radio.startListening();
					
		while ( radio.available(&pipe) ) 
		{
			len = radio.getDynamicPayloadSize();
			radio.read( receivePayload, len );

			// Display it on screen  , batt: 872 = 2.97V
                        timeInSeconds = time(NULL);

			if (pipe==1)
			  {
			    //time,sensorid,value,value,value....
			    if (argc == 1)
			      {
				fprintf(fd, "%d,%d,%s\n",timeInSeconds,pipe,receivePayload);
				fflush(fd);
			      }
			    else
			      printf("%d,%d,%s\n",timeInSeconds,pipe,receivePayload);

			    radio.stopListening();
			  }
			else
			  {
			    struct Payload *data = (struct Payload*)receivePayload;
			    struct tm sunRise, sunSet;

			    radio.stopListening();

			    if (data->receiveEnabled)
			      {
				radio.write( &timeInSeconds, sizeof(timeInSeconds));
			      }

			    sunRise.tm_sec = data->openTime.second;
			    sunRise.tm_min = data->openTime.minute;
			    sunRise.tm_hour = data->openTime.hour;
			    sunRise.tm_mday = data->openTime.day;
			    sunRise.tm_mon = data->openTime.month-1;
			    sunRise.tm_year = data->openTime.year+100;
			    sunRise.tm_isdst = -1;

			    sunSet.tm_sec = data->closeTime.second;
			    sunSet.tm_min = data->closeTime.minute;
			    sunSet.tm_hour = data->closeTime.hour;
			    sunSet.tm_mday = data->closeTime.day;
			    sunSet.tm_mon = data->closeTime.month-1;
			    sunSet.tm_year = data->closeTime.year+100;
			    sunSet.tm_isdst = -1;

			    //time,sensorid,value,value,value....
			    if (argc == 1)
			      {
				fprintf(fd, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
					timeInSeconds,
					pipe,
					data->nodeID,
					data->counter,
					data->openOrClosed,
					data->receiveEnabled,
					data->temperature,
					data->batteryVoltage,
					data->time,
					mktime(&sunRise),
					mktime(&sunSet));
				fflush(fd);
			      }
			    else
			      {
				printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
					timeInSeconds,
					pipe,
					data->nodeID,
					data->counter,
					data->openOrClosed,
					data->receiveEnabled,
					data->temperature,
					data->batteryVoltage,
					data->time,
					mktime(&sunRise),
					mktime(&sunSet));
			      }
			  }

			pipe++;
			
			// reset pipe to 0
			if ( pipe > 6 ) 
				pipe = 0;
		}
		
		delayMicroseconds(20);
	}
	if (argc == 1)
	  fclose(fd);
	// closeDB();
	return 0;
}

/*
modprobe spi-bcm2708
cd /tmp
chmod +x ./rf-gw
./rf-gw
 */
