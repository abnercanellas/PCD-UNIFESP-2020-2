#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

#define NUM_THREADS 4

int request = 0;
int respond = 0;
int soma = 0;

void *cliente (void* i)
{
    int n = (int) i;
    while (1)
    {
        //while (respond != n)
       // {
            request = n;
       // }
       
        int local = soma;
        sleep(rand()%2);
        soma = local + 1;
        respond = 0;

        printf("%d\n", soma);
    }

}

void *servidor (void* i)
{
    while (1)
    {
        while (request == 0){}
        respond = request;
        while (respond != 0){}
        request = 0;
    }
}

int main(void)
{

    	pthread_t t[NUM_THREADS];
	pthread_t t_servidor;
	int i;

    	pthread_create(&t_servidor, NULL, servidor, NULL); // Cria thread do servidor

	for(i = 0; i <= NUM_THREADS; i++){
		pthread_create(&t[i], NULL, cliente, (void*) (i+1));
	}
	
	pthread_exit(NULL);
	return 0;
}

