/*INCLUDES ************************************************************/
#include "timer.h"
#include "scheduler.h"
#include "util/atomic.h"
#include "uart.h"


/* PRIVATE VARIABLES **************************************************/
/** list of scheduled tasks */
static taskDescriptor* taskList = NULL;
static systemTime_t currentTime = 0;
/*FUNCTION DEFINITION *************************************************/
static void scheduler_update(void) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
	 currentTime++;				// keep track of time till scheduler started
	}

	taskDescriptor *temp;
	temp = taskList;
	while(temp != NULL)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		if(temp->expire-- <= 0){temp->execute = 1; temp->expire = temp->period;}
		temp = temp->next;
		}
	}
}

void scheduler_init() {
	sei();
	timer2_setCallback(&scheduler_update);
	timer2_start(); // Testing

}

void scheduler_run() {
	taskDescriptor *temp;
	temp = taskList;
	if(temp == NULL) {return;}
	while(1)
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		if(temp->execute == 1)   // flag for execution of task
		{
			temp->task(temp->param);  // execution of task with parameter provided
			temp->execute = 0;		  // making the execution flag '0' this will be set again by scheduler_update
			if(temp->period == 0) scheduler_remove(temp);
		}
		temp = temp->next;
		}
		if(temp == NULL) temp = taskList;
	}
}

bool scheduler_add(taskDescriptor * toAdd) {
	taskDescriptor *temp;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
	if (taskList != NULL)
	{
		temp = taskList;
		while(temp->next != NULL){
			if(temp == toAdd)return false;
			temp = temp->next;
		}
		temp->next = toAdd;
		temp = temp->next;
	}
	else{
		temp = toAdd;
		taskList = temp;
	}
	}
	temp->task = toAdd->task;
	temp->expire = toAdd->expire;
	temp->execute = 0;
	temp->period = toAdd->period;
	temp->param = toAdd->param;
	temp->next = NULL;
	return true;
}

void scheduler_remove(taskDescriptor * toRemove) {
	if(taskList == NULL) return;
	taskDescriptor *temp;
	temp = taskList;
	if(taskList == toRemove)
	{
		temp = taskList->next;
		free(taskList);
		taskList = temp;
		return;
	}
	else
	{
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
			while(!(temp->next == toRemove || temp->next == NULL))
		{
			temp = temp->next;
		}
		}
		if(temp->next != NULL){

			temp->next = temp->next->next;
			free(temp->next);
		}

	}
}


systemTime_t scheduler_getTime(){
	return currentTime;
}

void scheduler_setTime(systemTime_t time){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		currentTime = time;
	}
}
