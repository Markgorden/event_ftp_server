#include "condition.h"
#include <iostream>

using namespace std;

Condition::Condition()
{
	pthread_call("cond init", pthread_cond_init(&cond, NULL));
}	

Condition::~Condition()
{
	pthread_call("cond destroy", pthread_cond_destroy(&cond));
}

void Condition::notify()
{
	pthread_call("cond signal", pthread_cond_signal(&cond));
}

void Condition::wait(Mutex *mu)
{
	pthread_call("cond wait", pthread_cond_wait(&cond, &mu->mu_));
}

void Condition::broadcast()
{
	pthread_call("cond broadcast", pthread_cond_broadcast(&cond));
}
