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
#define OPTIONS                 "a:f:hm:t:vT:"

#define OPTION_DEFAULT_FILE     "data.dat"

#define OPTION_DEFAULT_AERROR                   (1e-3)
#define OPTION_DEFAULT_RERROR                   (1e-3)

#define OPTION_DEFAULT_TIMESTEP                 (1e+0)
#define OPTION_DEFAULT_END_TIME                 (1e+1)

#define OPTION_DEFAULT_MU                       (3.4)

typedef int (*system_callback)(double, const double *, double *, void *);

int rossler_cb(double t, const double y[], double dydt[], void *params);

void print_usage();

int solve_ode_system(gsl_odeiv2_system * sys, double y [], double eps_abs,
                     double eps_rel, double time_step, double time_end);

int main(int argc, char *const * argv)
{
    int retval = GSL_SUCCESS;
    char option = 0;
    int verbose = 0;

    char file_name[MAX_STRING_SIZE] = OPTION_DEFAULT_FILE;

    double end_time  = OPTION_DEFAULT_END_TIME;
    double time_step = OPTION_DEFAULT_TIMESTEP;
    double eps_rel   = OPTION_DEFAULT_RERROR;
    double eps_abs   = OPTION_DEFAULT_AERROR;

    double y[3]   = { 0, 0, 0};

    double params[] = { OPTION_DEFAULT_MU };

    gsl_odeiv2_system sys;

    while ((option = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (option)
        {
            case 'a':
                if (1 != sscanf(optarg, "%le", &eps_abs))
                {
                    fprintf(stderr, "Error: bad absolute error value. Should be number.\n");
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
                if (1 != sscanf(optarg, "%le", &params[0]))
                {
                    if (2.6 > params[0] && params[0] > 4.2)
                    {
                        fprintf(stderr, "Warning, value %f for mu will not produce classical attractor!\n", params[0]);
                    }
                    fprintf(stderr, "Error: bad mu value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 't':
                if (1 != sscanf(optarg, "%le", &time_step))
                {
                    fprintf(stderr, "Error: bad time step value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'v':
                verbose = 1;
            break;
            case 'T':
                if (1 != sscanf(optarg, "%le", &end_time))
                {
                    fprintf(stderr, "Error: bad end time value. Should be number.\n");
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
        printf("# Absolute error:                       %e\n", eps_abs);
        printf("# Relative error:                       %e\n", eps_rel);
        printf("# Time step:                            %e\n", time_step);
        printf("# End time:                             %f\n", end_time);
        printf("# Attractor parameter (mu)              %f\n", params[0]);
    }

    if (stdout != freopen(file_name, "w", stdout))
    {
        fprintf(stderr, "Error: could not open file %s", file_name);
        retval = GSL_FAILURE;
        goto done;
    }

    sys.function  = rossler_cb;
    sys.jacobian  = NULL;
    sys.dimension = 3;
    sys.params    = params;

    retval = solve_ode_system(&sys, y, eps_abs, eps_rel, time_step, end_time);
done:
    return retval;
}

int solve_ode_system(gsl_odeiv2_system * sys, double y[], double eps_abs,
                       double eps_rel, double time_step, double time_end)
{
    int retval = GSL_SUCCESS;
    gsl_odeiv2_driver * driver;
    double t = 0;
    size_t i, n;

    driver = gsl_odeiv2_driver_alloc_y_new(sys, gsl_odeiv2_step_rk8pd,
                                           DEFAULT_STEP, eps_abs, eps_rel);
    n = ceil(time_end / time_step);
    for (i = 1; i <= n; ++i)
    {
        retval = gsl_odeiv2_driver_apply(driver, &t, i * time_step, y);
        if (retval != GSL_SUCCESS)
        {
            fprintf(stderr, "Error: driver returned %d\n", retval);
            break;
        }
        printf("%.5e %.5e %.5e\n", y[0], y[1], y[2]);
    }
    gsl_odeiv2_driver_free(driver);
    return retval;
}

int rossler_cb(double t, const double y[], double dydt[], void *params)
{
    double mu = ((double *)params)[0];

    UNUSED(t);

    dydt[0] = - (y[1] + y[2]);
    dydt[1] = y[0] + 0.2 * y[1];
    dydt[2] = 0.2 + y[2] * (y[0] - mu);

    return GSL_SUCCESS;
}

void print_usage()
{
    printf("OVERVIEW: Produces data to Rössler Attractor.\n\n");
    printf("USAGE: rossler [options]\n\n");
    printf("OPTIONS:\n");
    printf("  -a <error>     Absolute error. Default is %e\n", OPTION_DEFAULT_RERROR);
    printf("  -f <file>      Output file. Default is " OPTION_DEFAULT_FILE "\n");
    printf("  -h             Print this message\n");
    printf("  -m <value>     Attractor parameter (mu). Default is %e\n", OPTION_DEFAULT_MU);
    printf("  -t <time_step> Time step. Default is %e\n", OPTION_DEFAULT_TIMESTEP);
    printf("  -v             Verbose mode\n");
    printf("  -T <time>      End time. Default is %e\n", OPTION_DEFAULT_END_TIME);
}
