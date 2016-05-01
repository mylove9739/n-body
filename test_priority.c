#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
	int rank,numprocess;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &numprocess );


	if(rank==0){ //master

		MPI_Status s;
		int Sstate=1;
		int Rstate;

		int p;
		for(p=1;p<numprocess;p++){

		    fflush(stdout);printf("master : order P%d to start reading\n",p);
		    MPI_Send(&Sstate, sizeof(int), MPI_INT, p, 20, MPI_COMM_WORLD);

		    MPI_Recv(&Rstate,sizeof(int),MPI_INT,p,21,MPI_COMM_WORLD,&s);
		    fflush(stdout);printf("master : P%d finished reading\n",p);
		}

	}
	else{ //workers

		int state; MPI_Status s;
		MPI_Recv(&state,sizeof(int),MPI_INT,0,20,MPI_COMM_WORLD,&s);

		//read here
		//sleep(1000);

		//send to master : finish reading
		state=2;
		MPI_Send(&state, sizeof(int), MPI_INT, 0, 21, MPI_COMM_WORLD);

		//processing
		//sleep(3000);
		fflush(stdout);printf("worker %d ended processing\n",rank);
	}

	MPI_Finalize();
	return 0;
}

