// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio de los fumadores
//
// *****************************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>      // incluye "time(....)"
#include <unistd.h>    // incluye "usleep(...)"
#include <stdlib.h>    // incluye "rand(...)" y "srand"

using namespace std ;
const unsigned int num_fumadores=3;
sem_t (puede_fumar[num_fumadores]);
sem_t puede_producir;
// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)

void retraso_aleatorio( const float smin, const float smax )
{
  static bool primera = true ;
  if ( primera )        // si es la primera vez:
  {  srand(time(NULL)); //   inicializar la semilla del generador
     primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}

// ----------------------------------------------------------------------------
// función que simula la acción de fumar, como un retardo aleatorio de la hebra.
// recibe como parámetro el numero de fumador
// el tiempo que tarda en fumar está entre dos y ocho décimas de segundo.

void fumar( int num_fumador )
{
   cout << "Fumador número " << num_fumador << ": comienza a fumar." << endl << flush ;
   sem_post(&puede_producir);
   retraso_aleatorio( 0.2, 0.8 );
   cout << "Fumador número " << num_fumador << ": termina de fumar." << endl << flush ;
}
// ----------------------------------------------------------------------------
void produce(int ingrediente){

	switch (ingrediente){
		case 0:
			cout << "Produce cerillas\n ";
			
		break;
			
		case 1:
			cout << "Produce papel\n";
			
			break;
		
		case 2:
			cout << "Produce tabaco\n";
			
		break;
	}
}
//-----------------------------------------------------------------------------
void * funcion_estanquero( void * ) {
	int ingrediente;
	while(true){
	sem_wait(&puede_producir);	//vemos si han retirado el ingrediente
	ingrediente=rand()%3;
	produce(ingrediente);
	sem_post(&(puede_fumar[ingrediente]));	//damos paso para que uno de los fumadores coja el ingrediente
	}

}

//-----------------------------------------------------------------------------

void * funcion_fumador( void * num_fum_void ) {
	while(true){
	unsigned long arg_fum = ( unsigned long ) num_fum_void ; // convertir puntero a entero
	sem_wait(&(puede_fumar[arg_fum]));			//vemos si puede fumar
	fumar(arg_fum+1);					//fuma
	}
}
// ----------------------------------------------------------------------------

int main()
{
  srand( time(NULL) ); // inicializa semilla aleatoria para selección aleatoria de fumador
	//semaforos
for(int i=0;i<num_fumadores;i++)
	sem_init(&(puede_fumar[i]),0,0);

sem_init (&puede_producir,0,1); //debe empezar el productor

  // creación y lanzamiento de hebras
  pthread_t estanquero;
  pthread_t id_fumadores[num_fumadores];

  pthread_create(&estanquero,NULL,funcion_estanquero,NULL);
for( unsigned long i = 0 ; i < num_fumadores ; i++ ){
	void * arg_ptr = (void *) i ; // convertir entero a puntero
	pthread_create(&(id_fumadores[i]), NULL, funcion_fumador, arg_ptr );
}
//que se esperen las hebras
pthread_join(estanquero,NULL);
for(int i=0;i<num_fumadores;i++)
	pthread_join(id_fumadores[i],NULL);

  return 0 ;
}
