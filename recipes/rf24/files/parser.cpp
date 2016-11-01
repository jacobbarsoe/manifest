#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

using namespace std;

int main() {
    //sim part
    char receivePayload[32];
    string input_line;
    int len,pipe=1;

    
    time_t timeInSeconds;

    while(cin) {
      //sim part
        getline(cin, input_line);
	strcpy (receivePayload, input_line.c_str());
	len = strnlen(receivePayload,32);

	timeInSeconds = time(NULL);

	//time,sensorid,value,value,value....
	printf("%d,%d,%s\n",timeInSeconds,pipe,receivePayload);
	sleep(1);
    };
    return 0;
}
