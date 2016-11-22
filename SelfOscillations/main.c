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
#define OPTIONS                 "a:f:hp:r:t:vw:D:J:K:P:R:S:T:"

#define OPTION_DEFAULT_FILE     "data.dat"

#define OPTION_DEFAULT_PR_MAX_RAT               (1.0) /* w */
#define OPTION_DEFAULT_PR_TIME_PREY_SEEK        (1.0) /* D */
#define OPTION_DEFAULT_PR_BR                    (1.0) /* S */
#define OPTION_DEFAULT_PR_EAT_RATIO             (1.0) /* J */
#define OPTION_DEFAULT_PREY_CARR_CAPAC          (1.0) /* K */
#define OPTION_DEFAULT_PREY_BR                  (1.0) /* r  */

#define OPTION_DEFAULT_INIT_PR_POPULATION       (5.0)
#define OPTION_DEFAULT_INIT_PREY_POPULATION     (5.0)

#define OPTION_DEFAULT_AERROR                   (1e-3)
#define OPTION_DEFAULT_RERROR                   (1e-3)

#define OPTION_DEFAULT_TIMESTEP                 (1e+0)
#define OPTION_DEFAULT_END_TIME                 (1e+1)

typedef int (*system_callback)(double, const double *, double *, void *);

int holling_tanner_cb(double t, const double y[], double dydt[], void *params);
int holling_tanner_jac_cb(double t, const double y[], double * dfdy,
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

    double r = OPTION_DEFAULT_PREY_BR;
    double K = OPTION_DEFAULT_PREY_CARR_CAPAC;
    double w = OPTION_DEFAULT_PR_MAX_RAT;
    double D = OPTION_DEFAULT_PR_TIME_PREY_SEEK;
    double S = OPTION_DEFAULT_PR_BR;
    double J = OPTION_DEFAULT_PR_EAT_RATIO;

    double y[2]   = {
                        OPTION_DEFAULT_INIT_PREY_POPULATION,
                        OPTION_DEFAULT_INIT_PR_POPULATION
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
            case 'w':
                if (1 != sscanf(optarg, "%le", &w))
                {
                    fprintf(stderr, "Error: bad predator maximum rate value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'D':
                if (1 != sscanf(optarg, "%le", &D))
                {
                    fprintf(stderr, "Error: bad Predator's prey seek time value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'J':
                if (1 != sscanf(optarg, "%le", &J))
                {
                    fprintf(stderr, "Error: bad predator eat ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'K':
                if (1 != sscanf(optarg, "%le", &K))
                {
                    fprintf(stderr, "Error: bad prey carrying capacity value. Should be number.\n");
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
            case 'R':
                if (1 != sscanf(optarg, "%le", &r))
                {
                    fprintf(stderr, "Error: bad prey birth ratio value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'S':
                if (1 != sscanf(optarg, "%le", &S))
                {
                    fprintf(stderr, "Error: bad predator birth ratio value. Should be number.\n");
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
        printf("# Prey initial population:              %f\n", y[0]);
        printf("# Predator initial population:          %f\n", y[1]);
        printf("# Predator maximum ratio    (w):        %f\n", w);
        printf("# Predator's prey seek time (D):        %f\n", D);
        printf("# Predator birth ratio      (S):        %f\n", S);
        printf("# Predator eat ratio        (J):        %f\n", J);
        printf("# Prey birth ratio          (r):        %f\n", r);
        printf("# Prey carrying capacity    (K):        %f\n", K);
    }

    if (stdout != freopen(file_name, "w", stdout))
    {
        fprintf(stderr, "Error: could not open file %s", file_name);
        retval = GSL_FAILURE;
        goto done;
    }

    params[0] = r;
    params[1] = K;
    params[2] = w;
    params[3] = D;
    params[4] = S;
    params[5] = J;

    sys.function = holling_tanner_cb;
    sys.jacobian = holling_tanner_jac_cb;
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

int holling_tanner_cb(double t, const double y[], double dydt[], void *params)
{
    double r = ((double *)params)[0];
    double K = ((double *)params)[1];
    double w = ((double *)params)[2];
    double D = ((double *)params)[3];
    double S = ((double *)params)[4];
    double J = ((double *)params)[5];

    UNUSED(t);

    dydt[0] = r * (1 - y[0] / K) * y[0] - w * y[0] * y[1] / (D + y[0]);
    dydt[1] = S * (1 - J * y[1] / y[0]) * y[1];

    return GSL_SUCCESS;
}

int holling_tanner_jac_cb(double t, const double y[], double * dfdy,
                         double dydt[], void *params)
{
    double r = ((double *)params)[0];
    double K = ((double *)params)[1];
    double w = ((double *)params)[2];
    double D = ((double *)params)[3];
    double S = ((double *)params)[4];
    double J = ((double *)params)[5];

    gsl_matrix_view dfdy_view =
        gsl_matrix_view_array(dfdy, 2, 2);
    gsl_matrix * jacobian_matrix = &dfdy_view.matrix;

    UNUSED(t);

    gsl_matrix_set(jacobian_matrix, 0, 0,
                   r - 2 * y[0] / K - D * w * y[0] / gsl_pow_2(D + y[1]));
    gsl_matrix_set(jacobian_matrix, 0, 1,
                   -w * y[0] / (D + y[0]));
    gsl_matrix_set(jacobian_matrix, 1, 0,
                   J * S * gsl_pow_2(y[1]) / gsl_pow_2(y[0]));
    gsl_matrix_set(jacobian_matrix, 1, 1,
                   S - 2 * J * S * y[1] / y[0]);

    dydt[0] = dydt[1] = 0.0;

    return GSL_SUCCESS;
}

/* "a:f:hp:r:t:vA:B:C:D:P:T:" */
void print_usage()
{
    printf("OVERVIEW: Holling-Tanner predator-prey model simulation.\n\n");
    printf("USAGE: holling-tanner [options]\n\n");
    printf("OPTIONS:\n");
    printf("  -a <error>     Absolute error. Default is %e\n", OPTION_DEFAULT_RERROR);
    printf("  -f <file>      Output file. Default is " OPTION_DEFAULT_FILE "\n");
    printf("  -h             Print this message\n");
    printf("  -p <value>     Predator initial population. Default is %f\n", OPTION_DEFAULT_INIT_PR_POPULATION);
    printf("  -r <error>     Relative error. Default is %e\n", OPTION_DEFAULT_RERROR);
    printf("  -t <time_step> Time step. Default is %e\n", OPTION_DEFAULT_TIMESTEP);
    printf("  -v             Verbose mode\n");
    printf("  -w <value>     Maximum rate of predator. Default is %f\n", OPTION_DEFAULT_PR_MAX_RAT);
    printf("  -D <value>     Time required for predator to search and find a prey. Default is %f\n", OPTION_DEFAULT_PR_TIME_PREY_SEEK);
    printf("  -J <value>     Number of prey required to support one predator at equilibrium. Default is %f\n", OPTION_DEFAULT_PR_EAT_RATIO);
    printf("  -K <value>     Carrying capacity of prey. Default is %f\n", OPTION_DEFAULT_PREY_CARR_CAPAC);
    printf("  -R <value>     Prey birth ratio. Default is %f\n", OPTION_DEFAULT_PREY_BR);
    printf("  -S <value>     Predator birth ragtio. Default is %f\n", OPTION_DEFAULT_PR_BR);
    printf("  -T <time>      End time. Default is %e\n", OPTION_DEFAULT_END_TIME);
    printf("  -P <value>     Prey initial population. Default is %f\n", OPTION_DEFAULT_INIT_PREY_POPULATION);
}
