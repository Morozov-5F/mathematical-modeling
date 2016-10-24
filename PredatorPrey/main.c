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
#define OPTIONS                 "a:f:hp:r:t:vA:B:C:D:P:T:"

#define OPTION_DEFAULT_FILE     "data.dat"

#define OPTION_DEFAULT_PREY_BR                  (1.0)
#define OPTION_DEFAULT_PREY_ER                  (1.0)
#define OPTION_DEFAULT_PREDATOR_BR              (1.0)
#define OPTION_DEFAULT_PREDATOR_ER              (1.0)
#define OPTION_DEFAULT_INIT_PREY_POPULATION     (5.0)
#define OPTION_DEFAULT_INIT_PREDATOR_POPULATION (5.0)

#define OPTION_DEFAULT_AERROR                   (1e-3)
#define OPTION_DEFAULT_RERROR                   (1e-3)

#define OPTION_DEFAULT_TIMESTEP                 (1e+0)
#define OPTION_DEFAULT_END_TIME                 (1e+1)

typedef int (*system_callback)(double, const double *, double *, void *);

int lotka_volterra_cb(double t, const double y[], double dydt[], void *params);
int lotka_volterra_jac_cb(double t, const double y[], double * dfdy,
                         double dydt[], void *params);

void print_usage();

int solve_ode_system(gsl_odeiv2_system * sys, double y [], double eps_abs,
                     double eps_rel, double time_step, double time_end);

int main(int argc, char *const * argv)
{
    int retval = GSL_SUCCESS;
    char option = 0;
    int verbose = 0;

    char file_name[MAX_STRING_SIZE] = OPTION_DEFAULT_FILE;

    double params[4];

    double end_time  = OPTION_DEFAULT_END_TIME;
    double time_step = OPTION_DEFAULT_TIMESTEP;
    double eps_rel   = OPTION_DEFAULT_RERROR;
    double eps_abs   = OPTION_DEFAULT_AERROR;

    double predator_birth_ratio   = OPTION_DEFAULT_PREDATOR_BR;
    double predator_extinct_ratio = OPTION_DEFAULT_PREDATOR_ER;
    double prey_birth_ratio       = OPTION_DEFAULT_PREY_BR;
    double prey_extinct_ratio     = OPTION_DEFAULT_PREY_ER;

    double y[2]   = {
                        OPTION_DEFAULT_INIT_PREY_POPULATION,
                        OPTION_DEFAULT_INIT_PREDATOR_POPULATION
                    };

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
            case 'p':
                if (1 != sscanf(optarg, "%le", &y[0]))
                {
                    fprintf(stderr, "Error: bad prey initial population value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'r':
                if (1 != sscanf(optarg, "%le", &eps_rel))
                {
                    fprintf(stderr, "Error: bad relative error value. Should be number.\n");
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
            case 'A':
                if (1 != sscanf(optarg, "%le", &prey_birth_ratio))
                {
                    fprintf(stderr, "Error: bad prey birth ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'B':
                if (1 != sscanf(optarg, "%le", &prey_extinct_ratio))
                {
                    fprintf(stderr, "Error: bad prey extinct ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'C':
                if (1 != sscanf(optarg, "%le", &predator_extinct_ratio))
                {
                    fprintf(stderr, "Error: bad predator extinct ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'D':
                if (1 != sscanf(optarg, "%le", &predator_birth_ratio))
                {
                    fprintf(stderr, "Error: bad predator birth ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'P':
                if (1 != sscanf(optarg, "%le", &y[1]))
                {
                    fprintf(stderr, "Error: bad predator initial population value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
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
        printf("# Absolute error:               %e\n", eps_abs);
        printf("# Relative error:               %e\n", eps_rel);
        printf("# Time step:                    %e\n", time_step);
        printf("# End time:                     %f\n", end_time);
        printf("# Prey initial population:      %f\n", y[0]);
        printf("# Predator initial population:  %f\n", y[1]);
        printf("# Prey birth ratio:             %f\n", prey_birth_ratio);
        printf("# Prey extinct ratio:           %f\n", prey_extinct_ratio);
        printf("# Predator birth ratio:         %f\n", predator_birth_ratio);
        printf("# Predator extinct ratio:       %f\n", predator_extinct_ratio);
        printf("# File name:                    %s\n", file_name);
    }

    if (stdout != freopen(file_name, "w", stdout))
    {
        fprintf(stderr, "Error: could not open file %s", file_name);
        retval = GSL_FAILURE;
        goto done;
    }

    params[0] = prey_birth_ratio;
    params[1] = prey_extinct_ratio;
    params[2] = predator_extinct_ratio;
    params[3] = predator_birth_ratio;

    sys.function = lotka_volterra_cb;
    sys.jacobian = lotka_volterra_jac_cb;
    sys.dimension = 2;
    sys.params = params;

    retval = solve_ode_system(&sys, y, eps_abs, eps_rel, time_step, end_time);
done:
    return retval;
}

int solve_ode_system(gsl_odeiv2_system * sys, double y[], double eps_abs,
              		 double eps_rel, double time_step, double time_end)
{
    int retval = GSL_SUCCESS;
    double t = 0;
    size_t i, n;
    gsl_odeiv2_driver * driver;

    driver = gsl_odeiv2_driver_alloc_y_new(sys, gsl_odeiv2_step_rk8pd,
                                           DEFAULT_STEP, eps_abs, eps_rel);
    n = ceil(time_end / time_step);
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

int lotka_volterra_cb(double t, const double y[], double dydt[], void *params)
{
    double prey_birth_ratio       = ((double *)params)[0];
    double prey_extinct_ratio     = ((double *)params)[1];
    double predator_extinct_ratio = ((double *)params)[2];
    double predator_birth_ratio   = ((double *)params)[3];

    UNUSED(t);

    dydt[0] = y[0] * (prey_birth_ratio - prey_extinct_ratio * y[1]);
    dydt[1] = y[1] * (predator_extinct_ratio * y[0] - predator_birth_ratio);

    return GSL_SUCCESS;
}

int lotka_volterra_jac_cb(double t, const double y[], double * dfdy,
                         double dydt[], void *params)
{
    double prey_birth_ratio       = ((double *)params)[0];
    double prey_extinct_ratio     = ((double *)params)[1];
    double predator_extinct_ratio = ((double *)params)[2];
    double predator_birth_ratio   = ((double *)params)[3];

    gsl_matrix_view dfdy_view =
        gsl_matrix_view_array(dfdy, 2, 2);
    gsl_matrix * jacobian_matrix = &dfdy_view.matrix;

    UNUSED(t);

    gsl_matrix_set(jacobian_matrix, 0, 0,
                   prey_birth_ratio - prey_extinct_ratio * y[1]);
    gsl_matrix_set(jacobian_matrix, 0, 1,
                   -prey_extinct_ratio * y[0]);
    gsl_matrix_set(jacobian_matrix, 1, 0,
                   predator_birth_ratio * y[1]);
    gsl_matrix_set(jacobian_matrix, 1, 1,
                   predator_birth_ratio * y[0] - predator_extinct_ratio);

    dydt[0] = dydt[1] = 0.0;

    return GSL_SUCCESS;
}

/* "a:f:hp:r:t:vA:B:C:D:P:T:" */
void print_usage()
{
    printf("OVERVIEW: Pendulum oscillations modelling program.\n\n");
    printf("USAGE: pendulum [options]\n\n");
    printf("OPTIONS:\n");
    printf("  -a <error>     Absolute error. Default is %e\n", OPTION_DEFAULT_RERROR);
    printf("  -f <file>      Output file. Default is " OPTION_DEFAULT_FILE "\n");
    printf("  -h             Print this message\n");
    printf("  -p <value>     Prey initial population. Default is %lf\n", OPTION_DEFAULT_INIT_PREY_POPULATION);
    printf("  -r <error>     Relative error. Default is %e\n", OPTION_DEFAULT_RERROR);
    printf("  -t <time_step> Time step. Default is %e\n", OPTION_DEFAULT_TIMESTEP);
    printf("  -v             Verbose mode\n");
    printf("  -A <value>     Prey birth ratio. Default is %lf\n", OPTION_DEFAULT_PREY_BR);
    printf("  -B <value>     Prey extinct ratio. Default is %lf\n", OPTION_DEFAULT_PREY_ER);
    printf("  -C <value>     Predator birth ratio. Default is %lf\n", OPTION_DEFAULT_PREDATOR_BR);
    printf("  -D <value>     Predator extinct ratio. Default is %lf\n", OPTION_DEFAULT_PREDATOR_ER);
    printf("  -P <value>     Predator initial population. Default is %lf\n", OPTION_DEFAULT_INIT_PREDATOR_POPULATION);
    printf("  -T <time>      End time. Default is %e\n", OPTION_DEFAULT_END_TIME);
}
