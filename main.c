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

    int body_id;

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


// output all body infomation to console
void body_info(body b) {

    //printf("Body memory address: %p\n", (void*)&b);
    printf("Body ID : %d\n", b.body_id);
    printf("Mass: %e\n", b.mass);

    printf("Location X: %d\n", (int)b.position_x);
    printf("Location Y: %d\n", (int)b.position_y);

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
    int k;
    for (k = 0; k < MAX_BODY; k++) {
        body *b = & list_body[k];

        b->mass = DEFAULT_BODY_MASS;

        int x = rand() % MAX_X_SIZE;
        int y = rand() % MAX_Y_SIZE;

        b->position_x = (double) x;
        b->position_y = (double) y;

        b->velocity_x = 0;
        b->velocity_y = 0;

        b->force_x = 0;
        b->force_y = 0;

        b->body_id = k+1;

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
    if (a->body_id != b->body_id) {

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





int main(int   argc,    char *argv[]) {
    init_list_body_data();

    int rank;
    int size;

    MPI_Init (&argc, &argv);      /* starts MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */



    // I follow this topic
    // http://stackoverflow.com/questions/9864510/struct-serialization-in-c-and-transfer-over-mpi
    // alow MPI tranfer truct data type
    const int       num_of_item = 8;
    int             blocklengths[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype    types[8] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
    MPI_Datatype    mpi_body_type;
    MPI_Aint        offsets[8];

    offsets[0] = offsetof(body, position_x);
    offsets[1] = offsetof(body, position_y);
    offsets[2] = offsetof(body, velocity_x);
    offsets[3] = offsetof(body, velocity_y);
    offsets[4] = offsetof(body, force_x);
    offsets[5] = offsetof(body, force_y);
    offsets[6] = offsetof(body, mass);
    offsets[7] = offsetof(body, body_id);

    MPI_Type_create_struct(num_of_item, blocklengths, offsets, types, &mpi_body_type);
    MPI_Type_commit(&mpi_body_type);

    //
    // http://stackoverflow.com/questions/11246150/synchronizing-master-slave-model-with-mpi
    // for master/slave
    int j = 0;
    int i = 0;
    int k = 0;
    int num_of_move =100;
    int index = 0;
    int p;
    
    
    int steps = MAX_BODY / (size - 1);

    if (( MAX_BODY % (size - 1)) > 0) {
        steps++;
    }

    for (j = 0; j < num_of_move; j++) {

        //which number of moves  process complte!?
        if(rank == 0) {
            printf("Processed %d of %d\n", j, num_of_move);
        }
        for (k = 0; k < MAX_BODY; k ++) {

            index = 0;
            for (i = 0; i <  steps; i++ ) {

                if(rank == 0) { //master

                    for(p = 1; p < size; p++){

                        body a =  list_body[k];



                        body b =  list_body[index];
                        if (index >= MAX_BODY -1) {
                            b = list_body[MAX_BODY -1];
                        }
                        // send 2 body to calculate force
                        MPI_Send(&a, 1, mpi_body_type, p, 20, MPI_COMM_WORLD);
                        MPI_Send(&b, 1, mpi_body_type, p, 20, MPI_COMM_WORLD);

                        //recieve result
                        body b_recv;
                        MPI_Recv(&b_recv, 1, mpi_body_type, p, 21, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                        //update to list
                        list_body[k] = b_recv;

                        index++;


                    }

                }
                else { //workers

                    //receive 2 body to calculate force
                    body  a;
                    MPI_Recv(&a, 1, mpi_body_type, 0, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    body  b;
                    MPI_Recv(&b, 1, mpi_body_type, 0, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


                    // calculating....
                    update_body_force(&a, &b);
                    update_body_velocity(&a);
                    update_body_location(&a);


                    // finish! send body a to master
                    MPI_Send(&a, 1, mpi_body_type, 0, 21, MPI_COMM_WORLD);


                }
            }
        }

    }
    // finaly, use rank=0 (master) to output result
    if (rank == 0) {
        i = 0;
        for (i = 0; i < MAX_BODY; i++) {
            body_info(list_body[i]);
        }
    }
    MPI_Finalize();


    return EXIT_SUCCESS;
}
