/*
 * Function and type declarations for hw2.c
 *
 * Timothy Mason, CSCI 5229 Computer Graphics
 */

// a couple of handy precompiler macros to save some typing
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#define LEN 256  // Maximum length of text string
void Print(const char* format , ...);

#define LORENZ_SIZE 50000	// Number of points in the Lorenz Attractor

typedef struct tripoint *tpp;
typedef struct tripoint {
	double x;
	double y;
	double z;
	double dist;
} tripoint;

void calc_lorenz(double sigma, double beta, double rho);
