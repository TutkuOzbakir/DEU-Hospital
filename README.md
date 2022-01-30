# DEU-Hospital
DEU-Hospital is a hospital simulation developed using POSIX threads, mutexes and semaphores.

Hospital facilities:   
  -Registiration  
  -Restroom  
  -Cafe  
  -General Practitioner (GP)  
  -Pharmacy  
  -Blood Lab  
  -Surgery  
  
The patient is generated with a specific disease that requires either medicine or surgery.  

The patient goes to registration office and waits for an available desk to register.  

After registration, the patient goes to GP’s office and examined.  

After examination the patient is send to one of the three main departments.

If the patient is prescribed medicine by GP, they go to pharmacy and buy this drug,
waiting for an empty register if there is none.  

If the patient is asked to give blood for testing in blood lab, the patient goes to blood lab,
waits for an available assistant and gives blood.  

After given blood, the patient returns to GP and they are either prescribed medicine if
they need it and goes to pharmacy as has been shown above or not if they do not require
it.  

If the patient requires a surgery, patient waits for an operating room, a randomly selected
number or surgeons and nurses to be available and after all of them are acquired, the
surgery is performed. After surgery, the patient goes to GP and they are either prescribed
medicine and goes to pharmacy as has been shown above or not if they do not require it.  

During any of waiting period of previously given operations, if the patient is sufficiently
hungry or needs go to the restroom, they can buy food from hospital cafe or go to the hospital
restroom. At the end of every waiting period, these hunger and restroom meters are
increased by a random value between 1 and given global increase rates.
