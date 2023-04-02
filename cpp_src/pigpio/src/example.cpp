/**************************************************
Test Development for pressure Transducer Driver
Example Code and Usage

g++ example.cpp -o ex -lpigpio -I ~/src/include/
***************************************************/
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <pigpio.h>
using namespace std;

#include "pressure.h"

Pressure sensorSetup(Pressure);

int main(){

	float pmmhg0;   	 // data variables
	
	if (gpioInitialise() < 0)
   {
      fprintf(stderr, "pigpio initialisation failed\n");
      return 1;
   }

	Pressure sensor1(0); // initializes 2 pressure sensors on bus 1 and 2
	
	cout.setf( ios::fixed );

	// read data and display for the user, run 200 times
	for (int i=0; i<200; i++){
		pmmhg0=sensor1.readPressure_mmhg();
   		cout <<"Pressure0_hg:: "<< setprecision( 3 ) <<pmmhg0<<endl;
		usleep(50000);
	}
	printf("----------------------------------------------\n");
	return 0;
}

Pressure sensorSetup(Pressure mySensor){
/*	printf("----------------------------------------------\n");
	printf("Pressure Transducer Bus is:\t\t %d \n",mySensor.getBus());
	printf("Pressure Transducer Address is:\t\t %d \n",mySensor.getAddress());
	printf("Pressure Transducer min counts is:\t %d \n",mySensor.getCountsMin());
	printf("Pressure Transducer max counts is:\t %d \n",mySensor.getCountsMax());
	printf("Pressure Transducer min mBar is:\t %d \n",mySensor.getPressureMin());
	printf("Pressure Transducer max mBar is:\t %d \n",mySensor.getPressureMax());
	printf("----------------------------------------------\n");
*/
return mySensor;
};
