#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stddef.h>
#include <unistd.h>
#include <pthread.h>

#include <mpich/mpi.h>

//X11
#include <X11/Xlib.h> // X11 library headers
#include <X11/Xutil.h>
#include <X11/Xos.h>
#define X_RESN 800 /* x resolution */
#define Y_RESN 800 /* y resolution */

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
#define     DEFAULT_BODY_MASS   1000

//the size for window and using for create random position
#define     MAX_X_SIZE          800
#define     MAX_Y_SIZE          600

//softening parameter, avoid infinities ????
#define     SOFTEN              1999.0

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
    //make a center!!
    body *b = & list_body[0];
    b->mass += b->mass * 50;
    b->position_x = 400;
    b->position_y = 400;


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



Display * x11setup(Window *win, GC *gc, int width, int height)
{

    /* --------------------------- X11 graphics setup ------------------------------ */
    Display 		*display;
    unsigned int 	win_x,win_y, /* window position */
                    border_width, /* border width in pixels */
                    display_width, display_height, /* size of screen */
                    screen; /* which screen */

    char 			window_name[] = "N-Body Simulation", *display_name = NULL;
    unsigned long 	valuemask = 0;
    XGCValues 		values;

    XSizeHints 		size_hints;

    //Pixmap 		bitmap;
    //XPoint 		points[800];
    FILE 			*fopen ();//, *fp;
    //char 			str[100];

    XSetWindowAttributes attr[1];

    if ( (display = XOpenDisplay (display_name)) == NULL ) { /* connect to Xserver */
        fprintf (stderr, "Cannot connect to X server %s\n",XDisplayName (display_name) );
        exit (-1);
    }

    screen = DefaultScreen (display); /* get screen size */
    display_width = DisplayWidth (display, screen);
    display_height = DisplayHeight (display, screen);

    win_x = 0; win_y = 0; /* set window position */

    border_width = 4; /* create opaque window */
    *win = XCreateSimpleWindow (display, RootWindow (display, screen),
            win_x, win_y, width, height, border_width,
            WhitePixel (display, screen), BlackPixel (display, screen));

    size_hints.flags = USPosition|USSize;
    size_hints.x = win_x;
    size_hints.y = win_y;
    size_hints.width = width;
    size_hints.height = height;
    size_hints.min_width = 300;
    size_hints.min_height = 300;

    XSetNormalHints (display, *win, &size_hints);
    XStoreName(display, *win, window_name);

    *gc = XCreateGC (display, *win, valuemask, &values); /* create graphics context */

    XSetBackground (display, *gc, BlackPixel (display, screen));
    XSetForeground (display, *gc, WhitePixel (display, screen));
    XSetLineAttributes (display, *gc, 1, LineSolid, CapRound, JoinRound);

    attr[0].backing_store = Always;
    attr[0].backing_planes = 1;
    attr[0].backing_pixel = BlackPixel(display, screen);

    XChangeWindowAttributes(display, *win, CWBackingStore | CWBackingPlanes | CWBackingPixel, attr);

    XSelectInput(display, *win, KeyPressMask);

    XMapWindow (display, *win);
    XSync(display, 0);

    /* --------------------------- End of X11 graphics setup ------------------------------ */
    return display;
}


int main(int   argc,    char *argv[]) {
    init_list_body_data();


    int  nbodies, x, y;
    Window win; // initialization for a window
    GC gc; // graphics context
    Display *display = NULL;
    unsigned int width = X_RESN, height = Y_RESN; /* window size */
    clock_t start, end, elapsed;



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
    if(rank == 0)
    {
        display = x11setup(&win, &gc, width, height);
        // other setup code in the master
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
        if(rank == 0) {
            XClearWindow(display, win);
            for (i = 0; i < MAX_BODY; i++) { // draw the bodies on the display
                body b = list_body[i];
                // this function will draw a circle inside a 3x3 box with the upper left
                // corner at (x,y). N.b. the last 2 arguments mean that it will fill from
                // 0 to 360 degrees - a full circle
                if (i == 0) {
                    XFillArc(display, win, gc, b.position_x , b.position_y, 6, 6, 0, 23040);
                }
                XFillArc(display, win, gc, b.position_x , b.position_y, 3, 3, 0, 23040);

                // you campicould also use XDrawPoint(display, win, gc, x, y) to draw a single
                // pixel at (x,y)
            }
            XFlush(display);
            //reset force
            for (i = 0; i < MAX_BODY; i++) {
                    body *b = & list_body[i];

                    b->force_x = 0;
                    b->force_y = 0;
            }

        }


        usleep(100 * 1000);

    }
    // finaly, use rank=0 (master) to output result
    if (rank == 0) {
        i = 0;
        for (i = 0; i < MAX_BODY; i++) {
            body_info(list_body[i]);
        }
    }



    // main loop
//    int running = 1; // loop variable
//    start = clock();
//    while(running) {

//        // checks to see if there have been any events,
//        // will exit the main loop if any key is pressed
//        if(rank==0) {
//            if(XPending(display)) {
//                XEvent ev;
//                XNextEvent(display, &ev);
//                switch(ev.type) {
//                    case KeyPress:
//                        running = 0;
//                        break;
//                }
//            }
//        }


        // your code to calculate the forces on the bodies goes here


//        end = clock();
//        elapsed = end - start;
//        // only update the display if > 1 millisecond has passed since the last update
//        if(elapsed / (CLOCKS_PER_SEC/1000) > 1 && rank==0) {
//            XClearWindow(display, win);
//            for (i = 0; i < MAX_BODY; i++) { // draw the bodies on the display
//                body b = list_body[i];
//                // this function will draw a circle inside a 3x3 box with the upper left
//                // corner at (x,y). N.b. the last 2 arguments mean that it will fill from
//                // 0 to 360 degrees - a full circle
//                XFillArc(display, win, gc, b.position_x , b.position_y, 3, 3, 0, 23040);

//                // you campicould also use XDrawPoint(display, win, gc, x, y) to draw a single
//                // pixel at (x,y)
//            }
//            start = end;
//            XFlush(display);
//        }
//    }

//    if(rank==0 && display) {
//        XCloseDisplay(display); // close the display window
//    }


    MPI_Finalize();


    return EXIT_SUCCESS;
}
