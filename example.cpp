/**************************************************
Test Development for pressure Transducer Driver
Example Code and Usage
***************************************************/
#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>

using namespace std;

#include "pressure.h"


Pressure sensorSetup(Pressure,int,int);


int main(){

	float pmbar, pmmhg0, pmmhg1;   	 // data variables

	Pressure sensor1(0), sensor2(1); // initializes 2 pressure sensors on bus 1 and 2

	cout.setf( ios::fixed );

	// read data and display for the user, run 200 times
	for (int i=0; i<200; i++){
		pmmhg0=sensor1.readPressure_mmhg();
		pmmhg1=sensor2.readPressure_mmhg();
   	cout <<"Pressure0_hg:: "<< setprecision( 3 ) <<pmmhg0<<"\tPressure1_hg:: "<<pmmhg1<<endl;
		usleep(50000);
	}

	printf("----------------------------------------------\n");
	return 0;
}

Pressure sensorSetup(Pressure mySensor, int bus, int i){
	printf("----------------------------------------------\n");
	printf("Pressure Transducer Number is:\t\t %d \n",i);
	printf("Pressure Transducer Bus is:\t\t %d \n",mySensor.bus);
	printf("Pressure Transducer Address is:\t\t %d \n",mySensor.address);
	printf("Pressure Transducer min counts is:\t %d \n",mySensor.countsmin);
	printf("Pressure Transducer max counts is:\t %d \n",mySensor.countsmax);
	printf("Pressure Transducer min mBar is:\t %d \n",mySensor.pmin);
	printf("Pressure Transducer max mBar is:\t %d \n",mySensor.pmax);
	printf("Pressure Transducer Format Descr is:\t %d \n",mySensor.fd);
return mySensor;
};
