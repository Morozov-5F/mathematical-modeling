#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

#define UNUSED(x) (void)(x)

#define DEFAULT_STEP            (1)
#define MAX_STRING_SIZE         (4096)
#define OPTIONS                 "a:e:f:hmo:r:t:A:V:"

#define OPTION_MODE_GENERAL     "general"
#define OPTION_MODE_HARMONIC    "harmonic"
#define OPTION_MODE_EXP         "exponential"

#define OPTION_DEFAULT_FILE     "data.dat"

#define OPTION_DEFAULT_OMEGA    (1e+0)
#define OPTION_DEFAULT_ANGLE    (1e-1)
#define OPTION_DEFAULT_VELOCITY (0.0)
#define OPTION_DEFAULT_AERROR   (1e-3)
#define OPTION_DEFAULT_RERROR   (1e-3)

#define OPTION_DEFAULT_TIMESTEP (1e+0)

typedef int (*system_callback)(double, const double *, double *, void *);

typedef struct option_mode_s
{
    char mode_name [MAX_STRING_SIZE];

} option_mode_t;

void print_usage();

int general_case_cb(double t, const double y[], double dydt[], void *params);
int small_angles_cb(double t, const double y[], double dydt[], void *params);
int balance_cb(double t, const double y[], double dydt[], void *params);

int solve_ode(gsl_odeiv2_system * sys, double y [],double eps_abs,
              double eps_rel, double time_end);

int main(int argc, char *const * argv)
{
    int retval = GSL_SUCCESS;
    char option = 0;
    double omega = OPTION_DEFAULT_OMEGA;
    double eps_rel = OPTION_DEFAULT_RERROR;
    double eps_abs = OPTION_DEFAULT_AERROR;
    double y[2] = { OPTION_DEFAULT_ANGLE, OPTION_DEFAULT_VELOCITY };
    system_callback function = general_case_cb;
    gsl_odeiv2_system sys;

    while ((option = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (option)
        {
            case 'a':
                if (1 != sscanf(optarg, "%le", &eps_abs))
                {
                    printf("Error: bad angle value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'e':
            break;
            case 'f':
                /* Set output file */
            break;
            case 'h':
                print_usage();
                retval = GSL_SUCCESS;
                goto done;
            break;
            case 'm':
                /* Implemet manual mode */
            break;
            case 'o':
                /* Set omega parameter */
            break;
            case 'r':
                /* Set relative error */
            break;
            case 't':
                /* Set timestamp */
            break;
            case 'A':
                /* Set initial angle */
            break;
            case 'V':
                /* Set initial velocity */
            break;
            default:
                print_usage();
                retval = GSL_EBADFUNC;
                goto done;
        }
    }

    sys.function = function;
    sys.jacobian = NULL;
    sys.dimension = 2;
    sys.params = &omega;

    retval = solve_ode(&sys, y, eps_abs, eps_rel, 10);
done:
    return retval;
}

int solve_ode(gsl_odeiv2_system * sys, double y[], double eps_abs,
              double eps_rel, double time_end)
{
    int retval = GSL_SUCCESS;
    double t = 0;
    size_t i;
    gsl_odeiv2_driver * driver;

    driver = gsl_odeiv2_driver_alloc_y_new(sys, gsl_odeiv2_step_rk4,
                                           DEFAULT_STEP, eps_abs, eps_rel);

    printf("%.5e %.5e %.5e\n", t, y[0], y[1]);
    for (i = 0; i < time_end; ++i)
    {
        retval = gsl_odeiv2_driver_apply(driver, &t, t + 1, y);
        if (retval != GSL_SUCCESS)
        {
            printf("error: driver returned %d\n", retval);
            break;
        }
        printf("%.5e %.5e %.5e\n", t, y[0], y[1]);
    }

    gsl_odeiv2_driver_free(driver);
    return retval;
}

int general_case_cb(double t, const double y[], double dydt[], void *params)
{
    double omega = *(double *)params;
    UNUSED(t);

    dydt[0] = y[1];
    dydt[1] = -gsl_pow_2(omega) * sin(y[0]);

    return GSL_SUCCESS;
}

int balance_cb(double t, const double y[], double dydt[], void *params)
{
    double omega = *(double *)params;
    UNUSED(t);

    dydt[0] = y[1];
    dydt[1] = gsl_pow_2(omega) * y[0];

    return GSL_SUCCESS;
}

int small_angles_cb(double t, const double y[], double dydt[], void *params)
{
    double omega = *(double *)params;
    UNUSED(t);

    dydt[0] = y[1];
    dydt[1] = -(omega * omega) * y[0];

    return GSL_SUCCESS;
}

void print_usage()
{
    printf("OVERVIEW: Pendulum oscillations modelling program.\n\n");
    printf("USAGE: pendulum [options]\n\n");
    printf("OPTIONS:\n");
    printf("  -a <error>     Absolute error. Default is %e\n", OPTION_DEFAULT_RERROR);
    printf("  -e <name>      Model to use: \n");
    printf("                   " OPTION_MODE_GENERAL  "\t- general case for every angle\n");
    printf("                   " OPTION_MODE_HARMONIC "\t- case for small angles (angle << 1)\n");
    printf("                   " OPTION_MODE_EXP      "\t- case for small angles around equilibrium point (|angle - PI| << 1)\n");
    printf("  -f <file>      Output file. Default is " OPTION_DEFAULT_FILE "\n");
    printf("  -h             Print this message\n");
    printf("  -m             Manual mode (not implemented yet)\n");
    printf("  -o <omega>     Omega parameter for system (angular frequency). Default is %e\n", OPTION_DEFAULT_OMEGA);
    printf("  -r <error>     Relative error. Default is %e\n", OPTION_DEFAULT_RERROR);
    printf("  -t <timestep>  Time step. Default is %e\n", OPTION_DEFAULT_TIMESTEP);
    printf("  -v             Verbose mode\n");
    printf("  -A <angle>     Initial angle (in radians). Default is %e\n", OPTION_DEFAULT_ANGLE);
    printf("  -V <velocity>  Initial velocity (in radians per second). Default is %e\n", OPTION_DEFAULT_VELOCITY);
}
