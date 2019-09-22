/*
 * Function and type declarations for hw2.c
 *
 * Timothy Mason, CSCI 5229 Computer Graphics
 */

// Handy precompiler macros to save some typing
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define SIGN_CHAR(x) ((x)>=0.0 ? '+' : '-')

#define LEN 256  // Maximum length of text string
void Print(const char* format , ...);

#define LORENZ_SIZE 50000	// Number of points in the Lorenz Attractor

// Default parameter values for the attractor.  These are the values studied by
// Edward Lorenz (source: https://en.wikipedia.org/wiki/Lorenz_system )
#define SIGMA_DEFAULT 10.0
#define BETA_DEFAULT (8.0 / 3.0)
#define RHO_DEFAULT 28.0

#define SIGMA_INCR 0.01		// Amount to change sigma on a single step
#define BETA_INCR 0.0033333	// Amount to change beta on a single step
#define RHO_INCR 0.05		// Amount to change rho on a single step

// Animation step sizes
#define AXIS_ANIM_STEP	0.5	// 1/2 degree per frame @ 60 FPS = 30 deg / sec
#define SIGMA_ANIM_STEP (SIGMA_INCR / 60.0)
#define BETA_ANIM_STEP (BETA_INCR / 60.0)
#define RHO_ANIM_STEP (RHO_INCR / 60.0)


// Data structure containing a single point in an attractor instance dataset
typedef struct tripoint {
	double x;
	double y;
	double z;
	double dist;	// Euclidian distance from the prior point (instantaneous velocity)
} tripoint;

typedef struct tripoint *tpp;

void calc_lorenz(double sigma, double beta, double rho);
