#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>

#include <mpich/mpi.h>
//define struct Body, typedef for alias of 'struct _body' to 'body'
typedef struct __body {

    //position x,y to display on graphic window
    double position_x;
    double position_y;

    //for movement speed!
    double velocity_x;
    double velocity_y;

    // the force, it will calculated for each time
    double force_x;
    double force_y;

    //
    double mass;

} body;


//the number of bodies:
#define     MAX_BODY            100

//default body mass
#define     DEFAULT_BODY_MASS   10000

//the size for window and using for create random position
#define     MAX_X_SIZE          800
#define     MAX_Y_SIZE          600

//softening parameter, avoid infinities ????
#define     SOFTEN              199.0

//
#define     TIME_STEP           10000.0

//the gravitational constant - https://en.wikipedia.org/wiki/Gravitational_constant
#define     GRAVITY             6.673e-11

//init bodies array with the MAX_BODY constant
body list_body[MAX_BODY];\
//cached global body
body  previous_body;

// output all body infomation to console
void body_info(body b) {

    printf("Mass: %e\n", b.mass);

    printf("X: %d\n", (int)b.position_x);
    printf("Y: %d\n", (int)b.position_y);

    printf("Force X: %e\n", b.force_x);
    printf("Force y: %e\n", b.force_y);

    printf("Velocity X: %e\n", b.velocity_x);
    printf("Velocity Y: %e\n", b.velocity_y);

    printf("\n");

}
//function for init data
void init_list_body_data() {
    //create bodies with random position
    srand(time(NULL));
    int i;
    for (i = 0; i < MAX_BODY; i++) {
        body *b = & list_body[i];

        b->mass = DEFAULT_BODY_MASS;

        int x = rand() % MAX_X_SIZE;
        int y = rand() % MAX_Y_SIZE;

        b->position_x = (double) x;
        b->position_y = (double) y;

        b->velocity_x = 0;
        b->velocity_y = 0;

        b->force_x = 0;
        b->force_y = 0;
    }
}

//void update_list_body_force() {

//    int i, j;
//    for (i = 0; i < MAX_BODY; i++) {

//        body *a = & list_body[i];

//        for (j = 0; j < MAX_BODY; j++) {

//            body *b = & list_body[j];

//            //check address prevent same body
//            if (a != b) {

//                double distance_x = a->position_x - b->position_x;
//                double distance_y = a->position_y - b->position_y;

//                double distance = sqrt((distance_x * distance_x) + (distance_y * distance_y));

//                double force = (GRAVITY * a->mass * b->mass )/ ((distance * distance) + (SOFTEN * SOFTEN));

//                double force_x = (force * distance_x / distance) ;
//                double force_y = force * distance_y / distance ;

//                a->force_x += force_x;
//                a->force_y += force_y;
//            }
//        }
//    }
//}


// a for current body, b for previous body
void update_body_force(body *a, body *b) {

    //check address prevent same body
    if (a != b) {

        double distance_x = a->position_x - b->position_x;
        double distance_y = a->position_y - b->position_y;

        double distance = sqrt((distance_x * distance_x) + (distance_y * distance_y));

        double force = (GRAVITY * a->mass * b->mass )/ ((distance * distance) + (SOFTEN * SOFTEN));

        double force_x = (force * distance_x / distance) ;
        double force_y = force * distance_y / distance ;

        a->force_x += force_x;
        a->force_y += force_y;


    }
}
//void update_list_body_velocity() {
//    int i;
//    for (i = 0; i < MAX_BODY; i++) {
//        body *b = & list_body[i];

//        b->velocity_x += TIME_STEP * b->force_x / b->mass;
//        b->velocity_y += TIME_STEP * b->force_y / b->mass;
//    }
//}

// for MPI
void update_body_velocity(body *b) {
    b->velocity_x += TIME_STEP * b->force_x / b->mass;
    b->velocity_y += TIME_STEP * b->force_y / b->mass;

}

//void update_list_body_location() {
//    int i;
//    for (i = 0; i < MAX_BODY; i++) {
//        body *b = & list_body[i];

//        b->position_x -= TIME_STEP * b->velocity_x;
//        b->position_y -= TIME_STEP * b->velocity_y;
//    }
//}
// for MPI
void update_body_location(body *b) {

    b->position_x -= TIME_STEP * b->velocity_x;
    b->position_y -= TIME_STEP * b->velocity_y;

}

//void reset_list_body_force() {
//    int i;
//    for (i = 0; i < MAX_BODY; i++) {
//        body *b = & list_body[i];

//        b->force_x = 0;
//        b->force_y = 0;
//    }
//}
// for MPI
void reset_body_force(body *b) {
    b->force_x = 0;
    b->force_y = 0;
}




/* This is the master */
int master_io(MPI_Comm master_comm, MPI_Comm comm )

{
    //    int        i,j, size;
    //    char       buf[256];
    //    MPI_Status status;
    //    MPI_Comm_size( master_comm, &size );
    //    for (j=1; j<=2; j++) {
    //        for (i=1; i<size; i++) {
    //            MPI_Recv( buf, 256, MPI_CHAR, i, 0, master_comm, &status );
    //            fputs( buf, stdout );
    //        }
    //    }

    //    body send;
    //    send.force_x = 999;
    //    send.force_y = 777;

    //    const int dest = 1;
    //    MPI_Send(&send,   1, mpi_body_type, dest, tag, MPI_COMM_WORLD);
}

/* This is the slave */
int slave_io(MPI_Comm master_comm, MPI_Comm comm )

{
    //    char buf[256];
    //    int  rank;

    //    MPI_Comm_rank( comm, &rank );
    //    sprintf( buf, "Hello from slave %d\n", rank );
    //    MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm );

    //    sprintf( buf, "Goodbye from slave %d\n", rank );
    //    MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, 0, master_comm );
    //    MPI_Status status;
    //    const int src = 0;

    //    body *recv;

    //    MPI_Recv(&recv,   1, mpi_body_type, src, tag, MPI_COMM_WORLD, &status);
    //    printf("Rank %d: Received: body force x = %f force y = %f\n", rank,
    //           recv.force_x, recv.force_y);
    //    if (previous_body) {
    //        update_body_force(&recv, &previous_body);
    //        update_body_velocity(&recv);
    //        update_body_location(&recv);
    //    }
    //    return 0;
}

int main(int   argc,    char *argv[]) {

    //    int j = 0;
    //    int  step_of_move = 100;
    //    for (j = 0; j < step_of_move; j++) {
    //        //update data!
    //        update_list_body_force();
    //        update_list_body_velocity();
    //        update_list_body_location();


    //        //reset force for each update step
    //        reset_list_body_force();
    //    }


    int rank;
    int size;

    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */

    //printf( "Hello world from process %d of %d\n", rank, size );

    // I follow this topic
    // http://stackoverflow.com/questions/9864510/struct-serialization-in-c-and-transfer-over-mpi
    // to tranfer truct data type over mpi
    const int       num_of_item = 7;
    int             blocklengths[7] = {1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype    types[7] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype    mpi_body_type;
    MPI_Aint        offsets[7];

    offsets[0] = offsetof(body, position_x);
    offsets[1] = offsetof(body, position_y);
    offsets[2] = offsetof(body, velocity_x);
    offsets[3] = offsetof(body, velocity_y);
    offsets[4] = offsetof(body, force_x);
    offsets[5] = offsetof(body, force_y);
    offsets[6] = offsetof(body, mass);

    MPI_Type_create_struct(num_of_item, blocklengths, offsets, types, &mpi_body_type);
    MPI_Type_commit(&mpi_body_type);

    // and here
    // http://stackoverflow.com/questions/11246150/synchronizing-master-slave-model-with-mpi
    // for master/slave
    int j = 0;
    int i = 0;
    int  step_of_move = 2;
    int data = 0;
    for (j = 0; j < step_of_move; j++) {



        if(rank == 0) { //master

            int Rstate = 0;
            int Sstate = 0;
            int p;

            for(p = 1; p < size; p++){

                body b =  list_body[i];
                MPI_Send(&b, 1, mpi_body_type, p, 20, MPI_COMM_WORLD);
                //fflush(stdout);
                //printf("master : order P%d to start reading\n",p);
                body b_recv;
                MPI_Recv(&b_recv, 1, mpi_body_type, p, 21, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //fflush(stdout);
                //printf("master : P%d finished reading\n\n",p);
                printf("body[%d]: verlocity %f\n", i, b_recv.velocity_x);
                i++;

            }

        }
        else { //workers

            body  b;
            MPI_Recv(&b, 1, mpi_body_type, 0, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //printf("worker %d start processing\n", rank);

            //read here
            //sleep(1000);

            //send to master : finish reading
            b.velocity_x = 99999;
            MPI_Send(&b, 1, mpi_body_type, 0, 21, MPI_COMM_WORLD);

            //processing
            //sleep(3000);
            //fflush(stdout);


        }

    }


    //printf("Data: %d\n", data);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
