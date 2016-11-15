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
#define OPTIONS                 "a:f:hp:r:t:vA:B:C:D:E:F:P:T:"

#define OPTION_DEFAULT_FILE     "data.dat"

#define OPTION_DEFAULT_FIRST_BR                 (1.0)
#define OPTION_DEFAULT_SECOND_BR                (1.0)
#define OPTION_DEFAULT_FIRST_ER                 (1.0)
#define OPTION_DEFAULT_SECOND_ER                (1.0)
#define OPTION_DEFAULT_FIRST_ICR                (1.0) /* Interspecies        */
#define OPTION_DEFAULT_SECOND_ICR               (1.0) /* competition ration  */
#define OPTION_DEFAULT_INIT_FIRST_POPULATION    (5.0)
#define OPTION_DEFAULT_INIT_SECOND_POPULATION   (5.0)

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

    double params[6];

    double end_time  = OPTION_DEFAULT_END_TIME;
    double time_step = OPTION_DEFAULT_TIMESTEP;
    double eps_rel   = OPTION_DEFAULT_RERROR;
    double eps_abs   = OPTION_DEFAULT_AERROR;

    double first_birth_ratio    = OPTION_DEFAULT_FIRST_BR;
    double first_extinct_ratio  = OPTION_DEFAULT_FIRST_ER;
    double second_birth_ratio   = OPTION_DEFAULT_SECOND_BR;
    double second_extinct_ratio = OPTION_DEFAULT_SECOND_ER;
    double first_ic_ratio       = OPTION_DEFAULT_FIRST_ICR;
    double second_ic_ratio      = OPTION_DEFAULT_SECOND_ICR;

    double y[2]   = {
                        OPTION_DEFAULT_INIT_FIRST_POPULATION,
                        OPTION_DEFAULT_INIT_SECOND_POPULATION
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
                    fprintf(stderr, "Error: bad first specie initial population value. Should be number.\n");
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
                if (1 != sscanf(optarg, "%le", &first_birth_ratio))
                {
                    fprintf(stderr, "Error: bad first specie birth ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'B':
                if (1 != sscanf(optarg, "%le", &first_extinct_ratio))
                {
                    fprintf(stderr, "Error: bad first specie extinct ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'C':
                if (1 != sscanf(optarg, "%le", &first_ic_ratio))
                {
                    fprintf(stderr, "Error: bad first specie interspecies competition ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'D':
                if (1 != sscanf(optarg, "%le", &second_birth_ratio))
                {
                    fprintf(stderr, "Error: bad second specie birth ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'E':
                if (1 != sscanf(optarg, "%le", &second_extinct_ratio))
                {
                    fprintf(stderr, "Error: bad second specie extinct ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'F':
                if (1 != sscanf(optarg, "%le", &second_ic_ratio))
                {
                    fprintf(stderr, "Error: bad second specie interspecies competition ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'P':
                if (1 != sscanf(optarg, "%le", &y[1]))
                {
                    fprintf(stderr, "Error: bad second specie initial population value. Should be number.\n");
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
        printf("# Absolute error:                       %e\n", eps_abs);
        printf("# Relative error:                       %e\n", eps_rel);
        printf("# Time step:                            %e\n", time_step);
        printf("# End time:                             %f\n", end_time);
        printf("# First specie initial population:      %f\n", y[0]);
        printf("# Second specie initial population:     %f\n", y[1]);
        printf("# First specie birth ratio:             %f\n", first_birth_ratio);
        printf("# First specie extinct ratio:           %f\n", first_extinct_ratio);
        printf("# First specie IC ratio:                %f\n", first_ic_ratio);
        printf("# Second specie birth ratio:            %f\n", second_birth_ratio);
        printf("# Second specie extinct ratio:          %f\n", second_extinct_ratio);
        printf("# Second specie IC ratio:               %f\n", second_ic_ratio);
        printf("# File name:                            %s\n", file_name);
    }

    if (stdout != freopen(file_name, "w", stdout))
    {
        fprintf(stderr, "Error: could not open file %s", file_name);
        retval = GSL_FAILURE;
        goto done;
    }

    params[0] = first_ic_ratio;
    params[1] = first_extinct_ratio;
    params[2] = first_birth_ratio;
    params[3] = second_ic_ratio;
    params[4] = second_extinct_ratio;
    params[5] = second_birth_ratio;

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
    gsl_odeiv2_driver * driver;
    double prevX = y[0];
    double prevY = y[1];
    double t = 0;
    size_t i, n;

    driver = gsl_odeiv2_driver_alloc_y_new(sys, gsl_odeiv2_step_rk8pd,
                                           DEFAULT_STEP, eps_abs, eps_rel);
    n = ceil(time_end / time_step);
    printf("%.5e %.5e %.5e ", t, y[0], y[1]);
    for (i = 1; i <= n; ++i)
    {
        double dx = 0, dy = 0;
        retval = gsl_odeiv2_driver_apply(driver, &t, i * time_step, y);
        if (retval != GSL_SUCCESS)
        {
            fprintf(stderr, "Error: driver returned %d\n", retval);
            break;
        }
        /* Ugly hack here to output information */
        dx = y[0] - prevX;
        dy = y[1] - prevY;
        prevX = y[0];
        prevY = y[1];
        printf("%.5e %.5e\n%.5e %.5e %.5e ", dx, dy, t, y[0], y[1]);
    }
    printf("%.5e %5.e\n", 0.0, 0.0);
    gsl_odeiv2_driver_free(driver);
    return retval;
}

int lotka_volterra_cb(double t, const double y[], double dydt[], void *params)
{
    double r1 = ((double *)params)[0];
    double b1 = ((double *)params)[1];
    double a1 = ((double *)params)[2];
    double r2 = ((double *)params)[3];
    double b2 = ((double *)params)[4];
    double a2 = ((double *)params)[5];

    UNUSED(t);

    dydt[0] = y[0] * (r1 - b1 * y[0] - a2 * y[1]);
    dydt[1] = y[1] * (r2 - b2 * y[1] - a1 * y[0]);

    return GSL_SUCCESS;
}

int lotka_volterra_jac_cb(double t, const double y[], double * dfdy,
                         double dydt[], void *params)
{
    double r1 = ((double *)params)[0];
    double b1 = ((double *)params)[1];
    double a1 = ((double *)params)[2];
    double r2 = ((double *)params)[3];
    double b2 = ((double *)params)[4];
    double a2 = ((double *)params)[5];

    gsl_matrix_view dfdy_view =
        gsl_matrix_view_array(dfdy, 2, 2);
    gsl_matrix * jacobian_matrix = &dfdy_view.matrix;

    UNUSED(t);

    gsl_matrix_set(jacobian_matrix, 0, 0,
                   r1 - 2 * b1 * y[0] - a2 * y[1]);
    gsl_matrix_set(jacobian_matrix, 0, 1,
                   -a2 * y[0]);
    gsl_matrix_set(jacobian_matrix, 1, 0,
                   -a1 * y[1]);
    gsl_matrix_set(jacobian_matrix, 1, 1,
                   r2 - 2 * b2 * y[1] - a1 * y[0]);


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
    printf("  -p <value>     First specie initial population. Default is %f\n", OPTION_DEFAULT_INIT_FIRST_POPULATION);
    printf("  -r <error>     Relative error. Default is %e\n", OPTION_DEFAULT_RERROR);
    printf("  -t <time_step> Time step. Default is %e\n", OPTION_DEFAULT_TIMESTEP);
    printf("  -v             Verbose mode\n");
    printf("  -A <value>     First specie birth ratio. Default is %f\n", OPTION_DEFAULT_FIRST_BR);
    printf("  -B <value>     First specie extinct ratio. Default is %f\n", OPTION_DEFAULT_FIRST_ER);
    printf("  -C <value>     First specie interspecies competition ratio. Default is %f\n", OPTION_DEFAULT_FIRST_ICR);
    printf("  -D <value>     Second specie birth ratio. Default is %f\n", OPTION_DEFAULT_SECOND_BR);
    printf("  -E <value>     Second specie extinct ratio. Default is %f\n", OPTION_DEFAULT_SECOND_ER);
    printf("  -F <value>     Second specie interspecies competition ratio. Default is %f\n", OPTION_DEFAULT_SECOND_ICR);
    printf("  -P <value>     Second specie initial population. Default is %f\n", OPTION_DEFAULT_INIT_SECOND_POPULATION);
    printf("  -T <time>      End time. Default is %e\n", OPTION_DEFAULT_END_TIME);
}
