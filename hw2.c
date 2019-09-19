/*
 *  Timothy Mason, homework 2, 3d Lorenz Attractor
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <float.h>
#include <math.h>

//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "hw2.h"
#include "hsv2rgb.h"

//  Globals
double th=0.0;                     // Azimuth of view angle (floating point for smoother animation)
double ph=0.0;                     // Elevation of view angle (floating point for smoother animation)

double sigma = SIGMA_DEFAULT;
double beta = BETA_DEFAULT;
double rho = RHO_DEFAULT;
//
// This value for dim was determined by programmatically checking the ranges of x, y, and z for the default 
// attractor parameters.  The maximum magnitude (26.0004 on the y axis) was  rounded to 30 then doubled to 
// get some aesthetically pleasing "breathing room" and avoid clipping to the ortho projection.
//
double dim=60.0;                // Dimension of orthogonal box
tripoint * arry = NULL;
double max_dist = 0.0;          // Track the magnitude of the largest vector for use in heatmap coloring

double th_dt = AXIS_ANIM_STEP * 2.0; // Default for the axis to be in motion
double ph_dt = AXIS_ANIM_STEP;

double sigma_dt = 0.0;
double beta_dt = 0.0;
double rho_dt = 0.0;

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *ch++);
}

/*
 *  Calculate an array of points for a lorenz attractor.  Code modified from
 *  provided example code lorenz.c
 *
 * * "The Lorenz attractor is an attractor that arises in a simplified system of equations describing the 
 * two-dimensional flow of fluid of uniform depth..." - http://mathworld.wolfram.com/LorenzAttractor.html
 */
void calc_lorenz(double s, double b, double r)
{
   int i;
   tpp p;      // Pointer to a tripoint struct - will be used to access the array of attractor data.

   /*  Coordinates  */
   double x = 1.0;
   double y = 1.0;
   double z = 1.0;

   /*  Attractor time step  */
   double dt = 0.001;
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0, p=arry; i<LORENZ_SIZE; i++, p++)
   {
      if (i) {
         double dx = (s*(y-x)) * dt;
         double dy = (x*(r-z)-y) * dt;
         double dz = (x*y - b*z) * dt;

         p->dist = sqrt(dx*dx + dy*dy + dz*dz);
         x += dx;
         y += dy;
         z += dz;
   
         max_dist = MAX(max_dist, p->dist);
      } else {
         // Initialize with the first point being zero distance
         p->dist = 0.0;
         max_dist = 0.0;
      }

      // Save the coordinates of the calculated point in the array
      p->x = x;
      p->y = y;
      p->z = z;
   }
   return;
}

/*
 *  Display the scene
 */
void display()
{
   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT);

   //  Reset previous transforms
   glLoadIdentity();

   //  Set view angle
   glRotated(ph, 1.0, 0.0, 0.0);
   glRotated(th, 0.0, 1.0, 0.0);

   // Draw the Lorenz attractor as an OpenGL line strip
   glBegin(GL_LINE_STRIP);
   tpp p = arry;
   for (int i=0; i<LORENZ_SIZE; i++, p++) {
      // Implement a "heatmap" coloring scheme which indicates velocity from one point to the next.  Blue is the slowest velocity,
      // up through red which is the maximum recorded velocity.
      HSV hsv;
      hsv.H = 360.0 * p->dist / max_dist;   // "heat" hue will be 0 (blue) for no movement from prior point, 360.0 (red) for largest of all recorded moves
      hsv.S = hsv.V = 1.0;
      RGB rgb = hsv2rgb(hsv);
      
      glColor3f(rgb.R, rgb.G, rgb.B);
      glVertex3d(p->x, p->y, p->z);
   }
   glEnd();

   //  Draw axes in light grey
   glColor3f(0.8,0.8,0.8);
   glBegin(GL_LINES);
   glVertex3d(0.0, 0.0, 0.0);   glVertex3d(dim*0.8, 0.0, 0.0);    // X axis
   glVertex3d(0.0, 0.0, 0.0);   glVertex3d(0.0, dim*0.8, 0.0);    // Y axis
   glVertex3d(0.0, 0.0, 0.0);   glVertex3d(0.0, 0.0, dim*0.8);    // Z axis
   glEnd();

   //  Label axes
   glRasterPos3d(dim*0.8, 0.0, 0.0);   Print("X");
   glRasterPos3d(0.0, dim*0.8, 0.0);   Print("Y");
   glRasterPos3d(0.0, 0.0, dim*0.8);   Print("Z");

   //  Display parameters
   glWindowPos2i(5,65);    Print("     Sigma:  %g", sigma);
   glWindowPos2i(5,45);    Print("      Beta:  %g", beta);
   glWindowPos2i(5,25);    Print("       Rho:  %g", rho);
   glWindowPos2i(5,5);     Print("View Angle:  (%0.1f,%0.1f)",th,ph);

   //  Flush and swap
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a regular key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   switch (ch) {
// Housekeeping
      case 27:    // Exit on ESC
         exit(0);

// Axes Manipulations
      case '0':   // Reset view angle and halt axes animations on 0
         th = ph = 0.0;
         th_dt = ph_dt = 0.0;

      case 'd':   // increase azimuth by 5 degrees
         th += 5.0;
         break;

      case 'a':   // decrease azimuth by 5 degrees   
         th -= 5.0;
         break;

      case 'w':   // increase elevation by 5 degrees
         ph += 5.0;
         break;

      case 's':   // decrease elevation by 5 degrees
         ph -= 5.0;
         break;

      case 'D':   // Increase azimuth velocity
         th_dt += AXIS_ANIM_STEP;
         break;

      case 'A':   // Decrease azimuth velocity
         th_dt -= AXIS_ANIM_STEP;
         break;

      case 'W':   // Increase elevation velocity
         ph_dt += AXIS_ANIM_STEP;
         break;

      case 'S':   // Decrease elevation velocity
         ph_dt -= AXIS_ANIM_STEP;
         break;


// Manipulation of attractor parameters
      case 'u':   // sigma++ single step
         sigma += SIGMA_INCR;
         calc_lorenz(sigma, beta, rho);

         break;

      case 'j':   // sigma-- single step
         sigma -= SIGMA_INCR;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'i':   // beta++ single step
         beta += BETA_INCR;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'k':   // beta-- single step
         beta -= BETA_INCR;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'o':   // rho++ single step
         rho += RHO_INCR;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'l':   // beta-- single step
         rho -= RHO_INCR;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'U':   // sigma increase velocity
         sigma_dt += SIGMA_ANIM_STEP;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'J':   // sigma decrease velocity
         sigma_dt -= SIGMA_ANIM_STEP;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'I':   // beta increase velocity
         beta_dt += BETA_ANIM_STEP;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'K':   // beta decrease velocity
         beta_dt -= BETA_ANIM_STEP;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'O':   // rho increase velocity
         rho_dt += RHO_ANIM_STEP;
         calc_lorenz(sigma, beta, rho);
         break;

      case 'L':   // beta decrease velocity
         rho_dt -= RHO_ANIM_STEP;
         calc_lorenz(sigma, beta, rho);
         break;

      case '-':   // Reset attractor parameters to defaults and halt parameter animations on -
         sigma = SIGMA_DEFAULT;
         beta = BETA_DEFAULT;
         rho = RHO_DEFAULT;
   
         sigma_dt = 0.0;
         beta_dt = 0.0;
         rho_dt = 0.0;

         calc_lorenz(sigma, beta, rho);
         break;
   }

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height>0.0) ? (double)width/height : 1.0;

   //  Set the viewport to the entire window
   glViewport(0, 0, width, height);

   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);

   //  Undo previous transformations
   glLoadIdentity();

   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h, +dim*w2h, -dim, +dim, -dim, +dim);

   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);

   //  Undo previous transformations
   glLoadIdentity();
}

/*
 * Called by GLUT library when idle (window not being resized or moved)
 *
 * Animation code adapted from example code in the book "OpenGL SuperBible", 4ed:  Richard S Wright, Jr,
 * Benjamin Lipchak, Nicholas Haemel.  Copyright (c) 2007 Pearson Education, Inc.  ISBN-13: 978-0-321-49882-3
 */
void TimerFunction(int value)    /* we only have a single timer, so the passed-in value is unused. */
{
   // Update axis rotation animation
   th += th_dt;
   ph += ph_dt;

   //  Keep angles to +/-360 degrees
   th = fmod(th, 360.0);
   ph = fmod(ph, 360.0);

   // Update attractor parameter animations
   sigma += sigma_dt;
   beta += beta_dt;
   rho += rho_dt;

   calc_lorenz(sigma, beta, rho);

   // Redraw the scene and prime the next call to this function to happen in 33msec (1000/33 ~= 30 Hz framerate)
   glutPostRedisplay();
   glutTimerFunc(33, TimerFunction, 1);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   
   //  Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   
   //  Request 900 x 900 pixel window
   glutInitWindowSize(900,900);
   
   //  Create the window
   glutCreateWindow("Timothy Mason");
   
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);

   // Tell GLUT to call "TimerFunction" in 33 msec (30.3 Hz framerate)
   glutTimerFunc(33, TimerFunction, 1);
   
   //  Allocate and calculate the initial Lorenz attractor
   arry = (tpp)malloc(sizeof(tripoint) * LORENZ_SIZE);

   if (arry) {
      // Precalculate the array so we know the range of deltas for heatmap coloring
      calc_lorenz(sigma, beta, rho);

      //  Pass control to GLUT so it can interact with the user
      glutMainLoop();
      
      //  Release the memory used to store the lorenz data
      free(arry);
      arry = NULL;

      //  Return code
      return 0;
   }
   else
   {
      fprintf(stderr, "ERROR! Unable to allocate memory to store the Lorenz attractor instance dataset.\n");
      return -1;
   }
}
