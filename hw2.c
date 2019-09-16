/*
 *  Timothy Mason, homework 2, 3d Lorenz Attractor
 *
 *  Display 2, 3 and 4 dimensional coordinates in 3D.
 *
 *  Key bindings:
 *  1      2D coordinates
 *  2      2D coordinates with fixed Z value
 *  3      3D coordinates
 *  4      4D coordinates
 *  +/-    Increase/decrease z or w
 *  arrows Change view angle
 *  0      Reset view angle
 *  ESC    Exit
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
int th=0;                     // Azimuth of view angle
int ph=0;                     // Elevation of view angle
//
// This value for dim was determined by programmatically checking the ranges of x, y, and z.  The maximum magnitude
// (26.0004 on the y axis) was then rounded to 30 then doubled to get some aesthetically pleasing "breathing room"
// and avoid clipping to the ortho projection.
//
double dim=60;                // Dimension of orthogonal box
tripoint * arry = NULL;
double max_dist = 0;          // Track the magnitude of the largest vector for use in heatmap coloring

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
      // glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
      glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *ch++);
}

/*
 *  Calculate an array of points for a lorenz attractor.  Code modified from
 *  provided example code lorenz.c
 *
 * * "The Lorenz attractor is an attractor that arises in a simplified system of // equations describing the 
 * two-dimensional flow of fluid of uniform depth..." - http://mathworld.wolfram.com/LorenzAttractor.html
 */
void calc_lorenz(double sigma, double beta, double rho)
{
   int i;
   tpp p;      // Pointer to a tripoint struct - will be used to access the array of lorenz data.

   /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;

   /*  Time step  */
   double dt = 0.001;
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0, p=arry; i<LORENZ_SIZE; i++, p++)
   {
      if (i) {
         double dx = (sigma*(y-x)) * dt;
         double dy = (x*(rho-z)-y) * dt;
         double dz = (x*y - beta*z) * dt;

         p->dist = sqrt(dx*dx + dy*dy + dz*dz);
         x += dx;
         y += dy;
         z += dz;
   
         max_dist = MAX(max_dist, p->dist);
      } else {
         p->dist = 0;
         max_dist = 0;
      }

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
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);

   // Draw the Lorenz attractor
   // Todo:  Change this to precompute the lorenz points only once per parameter set.
   glColor3f(0,1,1);
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

   //  Draw axes in light grey
   glColor3f(0.8,0.8,0.8);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);   glVertex3d(dim*0.8, 0, 0);    // X axis
   glVertex3d(0,0,0);   glVertex3d(0, dim*0.8, 0);    // Y axis
   glVertex3d(0,0,0);   glVertex3d(0, 0, dim*0.8);    // Z axis
   glEnd();

   //  Label axes
   glRasterPos3d(dim*0.8,0,0);   Print("X");
   glRasterPos3d(0,dim*0.8,0);   Print("Y");
   glRasterPos3d(0,0,dim*0.8);   Print("Z");

   //  Display parameters
   glWindowPos2i(5,5);     Print("View Angle=%d,%d",th,ph);

   //  Flush and swap
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   switch (ch) {
      case 27:    
         //  Exit on ESC
         exit(0);

      case 12:     
         //  Reset view angle on Ctrl-L
         th = ph = 0;
         break;

      // case '+':   
      //    //  Increase w by 0.1
      //    w += 0.1;
      //    break;

      // case '-':   
      //    //  Decrease w by 0.1
      //    w -= 0.1;
      //    break;
   }

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   switch (key) {
      case GLUT_KEY_RIGHT:    
         //  Right arrow key - increase azimuth by 5 degrees
         th += 5;
         break;

      case GLUT_KEY_LEFT:  
         //  Left arrow key - decrease azimuth by 5 degrees   
         th -= 5;
         break;

      case GLUT_KEY_UP:       
         //  Up arrow key - increase elevation by 5 degrees
         ph += 5;
         break;

      case GLUT_KEY_DOWN:     
         //  Down arrow key - decrease elevation by 5 degrees
         ph -= 5;
         break;
   }

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;

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
   
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   
   //  Allocate and calculate the initial Lorenz attractor
   arry = (tpp)malloc(sizeof(tripoint) * LORENZ_SIZE);

   if (arry) {
      calc_lorenz(10, 2.6666, 28);

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
      fprintf(stderr, "ERROR! Unable to allocate memory to store the Lorenz attractor dataset.\n");
      return -1;
   }
}
