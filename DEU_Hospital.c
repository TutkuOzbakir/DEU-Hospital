#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//Defining global variables.
int REGISTRATION_SIZE = 10;
int RESTROOM_SIZE = 10;
int CAFE_NUMBER = 10;
int GP_NUMBER = 10;
int PHARMACY_NUMBER = 10;
int BLOOD_LAB_NUMBER = 10;
int OR_NUMBER = 10;
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;
int SURGEON_LIMIT = 5; 			//Random between [1,5]
int NURSE_LIMIT = 5;    		//Random between [1,5]
int PATIENT_NUMBER = 1000;
int HOSPITAL_WALLET = 0;		//Total amount received from all patients at the end of the day.

//Defining global time variables.
int WAIT_TIME = 100;			//Random between [1,100]
int REGISTRATION_TIME = 100;	//Random between [1,100]
int GP_TIME = 200;				//Random between [1,200]
int PHARMACY_TIME = 100;		//Random between [1,100]
int BLOOD_LAB_TIME = 200;		//Random between [1,200]
int SURGERY_TIME = 500;			//Random between [1,500]
int CAFE_TIME = 100;			//Random between [1,100]
int RESTROOM_TIME = 100;		//Random between [1,100]

//Defining global cost variables.
int REGISTRATION_COST = 100;
int PHARMACY_COST = 200; 	//Random between [1,200]
int BLOOD_LAB_COST = 200;
int SURGERY_OR_COST = 200;
int SURGERY_SURGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200; 			//Random between [1,200]
int surgeryCost = 0;

//Defining rate variables.
int HUNGER_INCREASE_RATE = 10;		
int RESTROOM_INCREASE_RATE = 10;	

int Hunger_Meter = -1;			// Initialized between 1 and 100 at creation.
int Restroom_Meter = -1; 		// Initialized between 1 and 100 at creation.

int numberOfSurgeons = -1;		// Initialized between 1 and NURSE_LIMIT at surgery.
int numberOfNurses = -1;		// Initialized between 1 and SURGEON_LIMIT at surgery.


//Defining semaphores.
sem_t waiting;
sem_t registiration;
sem_t restroom;
sem_t cafe;
sem_t gpOffice;
sem_t pharmacy;
sem_t bloodLab;
sem_t operatingRoom;
sem_t surgery;
sem_t nurse;
sem_t surgeon;
sem_t cost;

//Function declarations.
void *People(void *num);
void *check(void *num, int hungerLimit, int restroomLimit, int amount);	//Function for checking hunger and restroom states.

int main(int argc, char *argv[]){
	
	int numPeople = 25;					//Number of people at the hospital.
	int id[numPeople]; 					//Will pass People function as id.
	int done = 0;
	pthread_t people[numPeople];
	
	//Initializing semaphores.
	sem_init(&waiting, 0, numPeople);
	sem_init(&registiration, 0, REGISTRATION_SIZE);
	sem_init(&restroom, 0, RESTROOM_SIZE);
	sem_init(&cafe, 0, CAFE_NUMBER);
	sem_init(&gpOffice, 0, GP_NUMBER);
	sem_init(&pharmacy, 0, PHARMACY_NUMBER);
	sem_init(&bloodLab, 0, BLOOD_LAB_NUMBER);
	sem_init(&operatingRoom, 0, OR_NUMBER);
	sem_init(&surgery, 0, 10); 			//Maximum 10 surgery can be performed at the same time.
	sem_init(&nurse, 0, 30);
	sem_init(&surgeon, 0, 30);
	sem_init(&cost, 0, numPeople);

	//Global variables' random values
	WAIT_TIME = rand() % WAIT_TIME + 1;						//Random between [1,100]
	REGISTRATION_TIME = rand() % REGISTRATION_TIME + 1;		//Random between [1,100]
	GP_TIME = rand() % GP_TIME + 1;							//Random between [1,200]
	PHARMACY_TIME = rand() % PHARMACY_TIME + 1;				//Random between [1,100]
	BLOOD_LAB_TIME = rand() % BLOOD_LAB_TIME + 1;			//Random between [1,200]
	SURGERY_TIME = rand() % SURGERY_TIME + 1;				//Random between [1,500]
	CAFE_TIME = rand() % CAFE_TIME + 1;						//Random between [1,100]
	RESTROOM_TIME = rand() % RESTROOM_TIME + 1;				//Random between [1,100]

	PHARMACY_COST = rand() % 200 + 1; 
	CAFE_COST = rand() % 200 + 1;


	//id parameter that will pass into people function.
	for(int i = 0; i < numPeople; i++){
		id[i] = i + 1;
	}
	
    //Creating people threads.
	for(int i = 0; i < numPeople; i++){
		pthread_create(&people[i], NULL, People, (void *)&id[i]);   
	}
	
    //Joining people threads.
	for (int i = 0; i < numPeople; i++) {
        pthread_join(people[i],NULL);
    }
	
	done = 1;
	printf("All patients are checked.\n");
	printf("Hospital wallet at the end of the day: %d TL\n",HOSPITAL_WALLET);
	return 0;
}


//Function for checking hunger and restroom states.	
void *check(void *num, int hungerLimit, int restroomLimit, int amount){
	int pId = *(int *)num;
	if(Hunger_Meter > hungerLimit){

		sem_wait(&waiting);
		printf("Patient number %d is waiting for cafe.\n",pId);
		usleep(WAIT_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE;
		Restroom_Meter += RESTROOM_INCREASE_RATE;	
		sem_post(&waiting);

		sem_wait(&cafe);
		printf("Patient number %d is in cafe desk number %d\n",pId,cafe);
		usleep(CAFE_TIME);
		Hunger_Meter = 1;
		Restroom_Meter += RESTROOM_INCREASE_RATE;
		amount += CAFE_COST;
		printf("Patient number %d is leaving cafe.\n",pId);
		sem_post(&cafe);
		
	}
	if(Restroom_Meter > restroomLimit){
		sem_wait(&waiting);
		printf("Patient number %d is waiting for restroom.\n",pId);
		usleep(WAIT_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE;
		Restroom_Meter += RESTROOM_INCREASE_RATE;	
		sem_post(&waiting);

		sem_wait(&restroom);
		printf("Patient number %d is in restroom number %d\n",pId,cafe);
		usleep(RESTROOM_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE;
		Restroom_Meter = 1;
		printf("Patient number %d is leaving restroom.\n",pId);
		sem_post(&restroom);
		
	}
}
void *People(void *num){

	int pId = *(int *)num;
    int operation = -1;				//GP result options. 3 choices: Pharmacy, blood lab or surgery.
	int opBlood = -1;				//After blood lab options.
	int opSurgery = -1;				//After surgery options.
	int limitHunger = 90; 			//After this number, patient will go to cafe.
	int limitRestroom = 80; 		//After this number, patient will go to restroom.
	HUNGER_INCREASE_RATE = rand() % HUNGER_INCREASE_RATE + 1;
	RESTROOM_INCREASE_RATE = rand() % RESTROOM_INCREASE_RATE +1;
	Hunger_Meter = rand() % 100 + 1;
	Restroom_Meter = rand() % 100 + 1;
	int payAmount = 0;

	sem_wait(&waiting);
	printf("Patient number %d is waiting for registiration.\n",pId);
	usleep(WAIT_TIME);
	Hunger_Meter += HUNGER_INCREASE_RATE;
	Restroom_Meter += RESTROOM_INCREASE_RATE;	
	sem_post(&waiting);

	check(num,limitHunger,limitRestroom,payAmount);

	sem_wait(&registiration);	
	printf("Patient number %d is registering in registiration desk number %d.\n",pId,registiration);
	usleep(REGISTRATION_TIME);
	Hunger_Meter += HUNGER_INCREASE_RATE;
	Restroom_Meter += RESTROOM_INCREASE_RATE;	
	payAmount += REGISTRATION_COST;
	printf("Patient number %d is leaving registiration.\n",pId);
	sem_post(&registiration);
	

	check(num,limitHunger,limitRestroom,payAmount);
	
    sem_wait(&gpOffice);		
    printf("Patient number %d is getting examined in GP Office number %d. \n",pId,gpOffice);
	usleep(GP_TIME);
	Hunger_Meter += HUNGER_INCREASE_RATE;
	Restroom_Meter += RESTROOM_INCREASE_RATE;

	check(num,limitHunger,limitRestroom,payAmount);

	operation = rand() % 3;
	if(operation == 0){				//Medicine. Patient will go to pharmacy.
		printf("Patient number %d is leaving GP Office. \n",pId);
		sem_post(&gpOffice);
		
		sem_wait(&waiting);
		printf("Patient number %d is waiting for Pharmacy.\n",pId);
		usleep(WAIT_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE; 
		Restroom_Meter += RESTROOM_INCREASE_RATE;
		sem_post(&waiting);

		check(num,limitHunger,limitRestroom,payAmount);

		sem_wait(&pharmacy);	
		printf("Patient number %d is registering in pharmacy desk number %d. \n",pId,pharmacy);
		usleep(PHARMACY_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE;	
		Restroom_Meter += RESTROOM_INCREASE_RATE;
		payAmount += PHARMACY_COST;
		printf("Patient number %d is leaving Pharmacy.\n",pId);
		sem_post(&pharmacy);

		check(num,limitHunger,limitRestroom,payAmount);

		sem_wait(&cost);
		printf("Patient number %d pays the amount of %d TL.\n",pId,payAmount);
		usleep(300);
		HOSPITAL_WALLET += payAmount;
		sem_post(&cost);

		printf("Patient number %d is going home after Pharmacy.\n",pId);
		
	}
	else if(operation == 1){			//Blood lab.

		sem_wait(&waiting);
		printf("Patient number %d is waiting for Blood Lab.\n",pId);
		usleep(WAIT_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE;	
		Restroom_Meter += RESTROOM_INCREASE_RATE;
		sem_post(&waiting);
		
		check(num,limitHunger,limitRestroom,payAmount);

		sem_wait(&bloodLab);	
		printf("Patient number %d is giving blood in blood lab number %d.\n",pId,bloodLab);
		usleep(BLOOD_LAB_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE; 
		Restroom_Meter += RESTROOM_INCREASE_RATE;
		payAmount += BLOOD_LAB_COST;
		printf("Patient number %d is leaving Blood Lab.\n",pId);
		sem_post(&bloodLab);

		check(num,limitHunger,limitRestroom,payAmount);
		printf("Patient number %d is going back to GP Office.\n",pId);

		//After blood lab, go back to GP's office. 2 choices: Go home or pharmacy.
		opBlood = rand() % 2;
		if(opBlood == 0){			//Go to pharmacy.
			printf("Patient number %d is leaving GP Office.\n",pId);
			sem_post(&gpOffice);
			
			sem_wait(&waiting);
			printf("Patient number %d is waiting for Pharmacy.\n",pId);
			usleep(WAIT_TIME);
			Hunger_Meter += HUNGER_INCREASE_RATE; 
			Restroom_Meter += RESTROOM_INCREASE_RATE;
			sem_post(&waiting);

			check(num,limitHunger,limitRestroom,payAmount);

			sem_wait(&pharmacy);	
			printf("Patient number %d is registering in pharmacy desk number %d. \n",pId);
			usleep(PHARMACY_TIME);
			Hunger_Meter += HUNGER_INCREASE_RATE; 		
			Restroom_Meter += RESTROOM_INCREASE_RATE;
			payAmount += PHARMACY_COST;
			printf("Patient number %d is leaving pharmacy.\n",pId);
			sem_post(&pharmacy);

			check(num,limitHunger,limitRestroom,payAmount);

			sem_wait(&cost);
			printf("Patient number %d pays the amount of %d TL.\n",pId,payAmount);
			usleep(300);
			HOSPITAL_WALLET += payAmount;
			sem_post(&cost);

			printf("Patient number %d is going home after Pharmacy.\n",pId);
		}
		else{				//Go home.
			printf("Patient number %d is leaving GP Office.\n",pId);
			sem_post(&gpOffice);
			check(num,limitHunger,limitRestroom,payAmount);

			sem_wait(&cost);
			printf("Patient number %d pays the amount of %d TL.\n",pId,payAmount);
			usleep(300);
			HOSPITAL_WALLET += payAmount;
			sem_post(&cost);
			
			printf("Patient number %d is going home.\n",pId);
		}
		
	}
	else if(operation == 2){	//Surgery.

		sem_wait(&waiting);
		printf("Patient number %d is waiting for Surgery.\n",pId);
		usleep(WAIT_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE; 
		Restroom_Meter += RESTROOM_INCREASE_RATE;
		sem_post(&waiting);

		check(num,limitHunger,limitRestroom,payAmount);

		sem_wait(&operatingRoom);  

		numberOfNurses = rand() % NURSE_LIMIT + 1;
		numberOfSurgeons = rand() % SURGEON_LIMIT + 1;
		for(int i = 0; i < numberOfNurses; i++){
			sem_wait(&nurse);
		}
		for(int i = 0; i < numberOfSurgeons; i++){
			sem_wait(&surgeon);
		}

		sem_wait(&surgery);
		printf("Patient number %d is getting surgery in opearting room number %d\n",pId,operatingRoom);
		usleep(SURGERY_TIME);
		Hunger_Meter += HUNGER_INCREASE_RATE;
		Restroom_Meter += RESTROOM_INCREASE_RATE;
		sem_post(&surgery);

		surgeryCost = SURGERY_OR_COST + numberOfNurses * SURGERY_NURSE_COST + numberOfSurgeons * SURGERY_SURGEON_COST;
		payAmount += surgeryCost;

		check(num,limitHunger,limitRestroom,payAmount);

		//Surgery is done. Post nurse and surgeon.
		for(int i = 0; i < numberOfNurses; i++){
			sem_post(&nurse);
		}
		for(int i = 0; i < numberOfSurgeons; i++){
			sem_post(&surgeon);
		}
		printf("Patient number %d leaving opearting room.\n",pId);
		sem_post(&operatingRoom);
		printf("Patient number %d is going back to GP office.\n",pId);

		//After surgery, go back to GP's office. 2 choices: Go home or pharmacy.
		opSurgery = rand() % 2;
		if(opSurgery == 0){				//Go to pharmacy.
			printf("Patient number %d leaving GP office.\n",pId);
			sem_post(&gpOffice);

			sem_wait(&waiting);
			printf("Patient number %d is waiting for Pharmacy.\n",pId);
			usleep(WAIT_TIME);
			Hunger_Meter += HUNGER_INCREASE_RATE;
			Restroom_Meter += RESTROOM_INCREASE_RATE;
			sem_post(&waiting);
			
			check(num,limitHunger,limitRestroom,payAmount);

			sem_wait(&pharmacy);	
			printf("Patient number %d is registering in pharmacy desk number %d. \n",pId,pharmacy);
			usleep(PHARMACY_TIME);
			Hunger_Meter += HUNGER_INCREASE_RATE;
			Restroom_Meter += RESTROOM_INCREASE_RATE;
			payAmount += PHARMACY_COST;
			printf("Patient number %d is leaving pharmacy.\n",pId);
			sem_post(&pharmacy);

			check(num,limitHunger,limitRestroom,payAmount);

			sem_wait(&cost);
			printf("Patient number %d pays the amount of %d TL\n",pId,payAmount);
			usleep(300);
			HOSPITAL_WALLET += payAmount;
			sem_post(&cost);

			printf("Patient number %d is going home after Pharmacy.\n",pId);
		}
		else{			//Go home.
			printf("Patient number %d leaving GP office.\n",pId);
			sem_post(&gpOffice);

			check(num,limitHunger,limitRestroom,payAmount);

			sem_wait(&cost);
			printf("Patient number %d pays the amount of %d TL\n",pId,payAmount);
			usleep(300);
			HOSPITAL_WALLET += payAmount;
			sem_post(&cost);

			printf("Patient number %d is going home.\n",pId);
			
		}
	}
	
	
}

