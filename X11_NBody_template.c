#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


#include <X11/Xlib.h> // X11 library headers
#include <X11/Xutil.h>
#include <X11/Xos.h>
#define X_RESN 800 /* x resolution */
#define Y_RESN 800 /* y resolution */

// function prototype
Display * x11setup(Window *win, GC *gc, int width, int height);

int main(int argc, char *argv[])
{
	int rank, nprocs, i, nbodies, x, y;
	Window win; // initialization for a window
	GC gc; // graphics context
	Display *display = NULL;
	unsigned int width = X_RESN, height = Y_RESN; /* window size */
	clock_t start, end, elapsed;
	

	MPI_Init(&argc, &argv); 
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs); 
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        
	if(rank==0)
	{
		display = x11setup(&win, &gc, width, height);
		// other setup code in the master
	}
	
	
	// any other required setup code for the slaves
	
	
	// main loop
	int running = 1; // loop variable
	start = clock();
	while(running) {
	
		// checks to see if there have been any events,
		// will exit the main loop if any key is pressed
		if(rank==0) {
			if(XPending(display)) {
				XEvent ev;
				XNextEvent(display, &ev);
				switch(ev.type) {
					case KeyPress:
						running = 0;
						break;
				}
			}
		}
		
		
		// your code to calculate the forces on the bodies goes here
		
		
		end = clock();
		elapsed = end - start;
		// only update the display if > 1 millisecond has passed since the last update
		if(elapsed / (CLOCKS_PER_SEC/1000) > 1 && rank==0) {
			XClearWindow(display, win);
			for(i=0; i<nbodies; i++) { // draw the bodies on the display
				
				// this function will draw a circle inside a 3x3 box with the upper left
				// corner at (x,y). N.b. the last 2 arguments mean that it will fill from
				// 0 to 360 degrees - a full circle
				XFillArc(display, win, gc, x, y, 3, 3, 0, 23040);
				
				// you campicould also use XDrawPoint(display, win, gc, x, y) to draw a single
				// pixel at (x,y)
			}
			start = end;
			XFlush(display);
		}
	}

	if(rank==0 && display) {
		XCloseDisplay(display); // close the display window
	}
	
	MPI_Finalize();
	return 0;
}


Display * x11setup(Window *win, GC *gc, int width, int height)/
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