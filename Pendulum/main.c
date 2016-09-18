#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#define __USE_POSIX2           (1)
#endif

#include <unistd.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

#define UNUSED(x) (void)(x)

#define DEFAULT_STEP            (1)
#define MAX_STRING_SIZE         (4096)
#define OPTIONS                 "a:e:f:hmo:r:t:vA:F:T:V:"

#define OPTION_MODE_GENERAL     "general"
#define OPTION_MODE_HARMONIC    "harmonic"
#define OPTION_MODE_EXP         "exponential"

#define OPTION_DEFAULT_FILE     "data.dat"

#define OPTION_DEFAULT_FRICTION (0.0)
#define OPTION_DEFAULT_OMEGA    (1e+0)
#define OPTION_DEFAULT_ANGLE    (1e-1)
#define OPTION_DEFAULT_VELOCITY (0.0)
#define OPTION_DEFAULT_AERROR   (1e-3)
#define OPTION_DEFAULT_RERROR   (1e-3)

#define OPTION_DEFAULT_TIMESTEP (1e+0)
#define OPTION_DEFAULT_END_TIME (1e+1)

typedef int (*system_callback)(double, const double *, double *, void *);

int general_case_cb(double t, const double y[], double dydt[], void *params);
int small_angles_cb(double t, const double y[], double dydt[], void *params);
int balance_cb(double t, const double y[], double dydt[], void *params);

typedef struct option_mode_s
{
    char mode_name[MAX_STRING_SIZE];
    system_callback mode_callback;
} option_mode_t;

static option_mode_t option_mode [] =
{
    { OPTION_MODE_GENERAL,  general_case_cb },
    { OPTION_MODE_HARMONIC, small_angles_cb },
    { OPTION_MODE_EXP,      balance_cb },
};

void print_usage();

int solve_ode(gsl_odeiv2_system * sys, double y [],double eps_abs,
              double eps_rel, double time_step, double time_end);

int main(int argc, char *const * argv)
{
    int retval = GSL_SUCCESS;
    int i = 0;
    int found = 0;
    char option = 0;
    int verbose = 0;

    char file_name[MAX_STRING_SIZE] = OPTION_DEFAULT_FILE;

    double params[2];

    double friction  = OPTION_DEFAULT_FRICTION;
    double end_time  = OPTION_DEFAULT_END_TIME;
    double time_step = OPTION_DEFAULT_TIMESTEP;
    double omega     = OPTION_DEFAULT_OMEGA;
    double eps_rel   = OPTION_DEFAULT_RERROR;
    double eps_abs   = OPTION_DEFAULT_AERROR;

    double y[2]      = { OPTION_DEFAULT_ANGLE, OPTION_DEFAULT_VELOCITY };

    system_callback function = general_case_cb;
    gsl_odeiv2_system sys;

    while ((option = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (option)
        {
            case 'a':
                if (1 != sscanf(optarg, "%le", &eps_abs))
                {
                    fprintf(stderr, "Error: bad absolute error value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'e':
                for (i = 0; i < sizeof(option_mode) / sizeof(option_mode_t);
                    ++i)
                {
                    if (!strcmp(optarg, option_mode[i].mode_name))
                    {
                        function = option_mode[i].mode_callback;
                        found = 1;
                        break;
                    }
                }
                if (!found)
                {
                    fprintf(stderr, "Error: bad mode value.");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'f':
                strcpy(file_name, optarg);
            break;
            case 'h':
                print_usage();
                retval = GSL_SUCCESS;
                goto done;
            break;
            case 'm':
                fprintf(stderr, "Manual mode is not implemented yet\n");
                retval = GSL_FAILURE;
                goto done;
            break;
            case 'o':
                if (1 != sscanf(optarg, "%le", &omega))
                {
                    fprintf(stderr, "Error: bad omega parameter value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'r':
                if (1 != sscanf(optarg, "%le", &eps_rel))
                {
                    fprintf(stderr, "Error: bad relative error value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 't':
                if (1 != sscanf(optarg, "%le", &time_step))
                {
                    fprintf(stderr, "Error: bad time step value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'v':
                verbose = 1;
            break;
            case 'A':
                if (1 != sscanf(optarg, "%le", &y[0]))
                {
                    fprintf(stderr, "Error: bad initial angle value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'F':
                if (1 != sscanf(optarg, "%le", &friction))
                {
                    fprintf(stderr, "Error: bad friction coefficient value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'T':
                if (1 != sscanf(optarg, "%le", &end_time))
                {
                    fprintf(stderr, "Error: bad end time value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'V':
                if (1 != sscanf(optarg, "%le", &y[1]))
                {
                    fprintf(stderr, "Error: bad initial velocity value. Value should be in scientific notation (i.e 1e+2)\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            default:
                print_usage();
                retval = GSL_EBADFUNC;
                goto done;
        }
    }

    if (verbose)
    {
        printf("# Absolute error:       %e\n", eps_abs);
        printf("# Relative error:       %e\n", eps_rel);
        printf("# Friction coefficient: %e\n", friction);
        printf("# Initial angle:        %e\n", y[0]);
        printf("# Initial velocity:     %e\n", y[1]);
        for (i = 0; i < sizeof(option_mode) / sizeof(option_mode_t); ++i)
        {
            if (function == option_mode[i].mode_callback)
            {
                printf("# Mode name:            %s\n", option_mode[i].mode_name);
            }
        }
        printf("# Time step:            %e\n", time_step);
        printf("# End time:             %f\n", end_time);
        printf("# File name:            %s\n", file_name);
    }

    if (stdout != freopen(file_name, "w", stdout))
    {
        fprintf(stderr, "Error: could not open file %s", file_name);
        retval = GSL_FAILURE;
        goto done;
    }

    params[0] = omega;
    params[1] = friction;

    sys.function = function;
    sys.jacobian = NULL;
    sys.dimension = 2;
    sys.params = params;

    retval = solve_ode(&sys, y, eps_abs, eps_rel, time_step, end_time);
done:
    return retval;
}

int solve_ode(gsl_odeiv2_system * sys, double y[], double eps_abs,
              double eps_rel, double time_step, double time_end)
{
    int retval = GSL_SUCCESS;
    double t = 0;
    size_t i, n;
    gsl_odeiv2_driver * driver;

    driver = gsl_odeiv2_driver_alloc_y_new(sys, gsl_odeiv2_step_rk4,
                                           DEFAULT_STEP, eps_abs, eps_rel);
    n = ceil(time_end / time_step);
    printf("%.5e %.5e %.5e\n", t, y[0], y[1]);
    for (i = 0; i <= n; ++i)
    {
        retval = gsl_odeiv2_driver_apply(driver, &t, i * time_step, y);
        if (retval != GSL_SUCCESS)
        {
            fprintf(stderr, "Error: driver returned %d\n", retval);
            break;
        }
        printf("%.5e %.5e %.5e\n", t, y[0], y[1]);
    }

    gsl_odeiv2_driver_free(driver);
    return retval;
}

int general_case_cb(double t, const double y[], double dydt[], void *params)
{
    double omega    = ((double *)params)[0];
    double friction = ((double *)params)[0];

    UNUSED(t);

    dydt[0] = y[1];
    dydt[1] = -gsl_pow_2(omega) * sin(y[0]) - friction * y[1];

    return GSL_SUCCESS;
}

int balance_cb(double t, const double y[], double dydt[], void *params)
{
    double omega    = ((double *)params)[0];
    double friction = ((double *)params)[0];

    UNUSED(t);

    dydt[0] = y[1];
    dydt[1] = gsl_pow_2(omega) * y[0] - friction * y[1];

    return GSL_SUCCESS;
}

int small_angles_cb(double t, const double y[], double dydt[], void *params)
{
    double omega    = ((double *)params)[0];
    double friction = ((double *)params)[0];

    UNUSED(t);

    dydt[0] = y[1];
    dydt[1] = -(omega * omega) * y[0] - friction * y[1];

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
    printf("  -t <time_step> Time step. Default is %e\n", OPTION_DEFAULT_TIMESTEP);
    printf("  -v             Verbose mode\n");
    printf("  -A <angle>     Initial angle (in radians). Default is %e\n", OPTION_DEFAULT_ANGLE);
    printf("  -F <coeff>     Friction coefficient. Default is %e\n", OPTION_DEFAULT_FRICTION);
    printf("  -T <time>      End time. Default is %e\n", OPTION_DEFAULT_END_TIME);
    printf("  -V <velocity>  Initial velocity (in radians per second). Default is %e\n", OPTION_DEFAULT_VELOCITY);
}
