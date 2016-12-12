#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#define __USE_POSIX2           (1)
#endif

#include <unistd.h>

#define UNUSED(x) (void)(x)

#define DEFAULT_STEP            (1)
#define MAX_STRING_SIZE         (4096)
#define OPTIONS                 "a:f:hr:vx:"

#define OPTION_DEFAULT_FILE     "data.dat"

#define OPTION_DEFAULT_X_POINTS                 (100)
#define OPTION_DEFAULT_R_POINTS                 (1000)

#define OPTION_DEFAULT_AERROR                   (1e-3)

double model_cb(double x[], void *params);

void print_usage();

int make_chaos(size_t x_points, size_t r_points, double eps_abs);

int main(int argc, char *const * argv)
{
    int retval = EXIT_SUCCESS;
    char option = 0;
    int verbose = 0;

    char file_name[MAX_STRING_SIZE] = OPTION_DEFAULT_FILE;

    double eps_abs = OPTION_DEFAULT_AERROR;

    size_t r_points = OPTION_DEFAULT_R_POINTS;
    size_t x_points = OPTION_DEFAULT_X_POINTS;

    while ((option = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (option)
        {
            case 'a':
                if (1 != sscanf(optarg, "%le", &eps_abs))
                {
                    fprintf(stderr, "Error: bad absolute error value. Should be number.\n");
                    retval = EXIT_FAILURE;
                    goto done;
                }
            break;
            case 'f':
                strcpy(file_name, optarg);
            break;
            case 'h':
                print_usage();
                retval = EXIT_FAILURE;
                goto done;
            break;
            case 'r':
                if (1 != sscanf(optarg, "%lu", &r_points))
                {
                    fprintf(stderr, "Error: bad number of \'r\' values. Should be number.\n");
                    retval = EXIT_FAILURE;
                    goto done;
                }
            break;
            case 'v':
                verbose = 1;
            break;
            case 'x':
                if (1 != sscanf(optarg, "%lu", &x_points))
                {
                    fprintf(stderr, "Error: bad number of \'x\' values. Should be number.\n");
                    retval = EXIT_FAILURE;
                    goto done;
                }
            break;
            default:
                print_usage();
                retval = EXIT_FAILURE;
                goto done;
        }
    }

    if (verbose)
    {
        printf("# Absolute error:                       %e\n", eps_abs);
        printf("# Number of \'x\' points:                 %lu\n", x_points);
        printf("# Number of \'r\' points                  %lu\n", r_points);
    }

    if (stdout != freopen(file_name, "w", stdout))
    {
        fprintf(stderr, "Error: could not open file %s", file_name);
        retval = EXIT_SUCCESS;
        goto done;
    }

    retval = make_chaos(x_points, r_points, eps_abs);
done:
    return retval;
}

int make_chaos(size_t x_points, size_t r_points, double eps_abs)
{
    int retval = EXIT_SUCCESS;
    double x[2];
    double r = 0, r_step = 1.0 / r_points;

    size_t i, j;

    for (i = 0; i < r_points; ++i, r += r_step)
    {
        x[0] = x[1] = 0.1;
        for (j = 0; j < x_points * 10; ++j)
        {
            model_cb(x, (void *)(&r));
            x[0] = x[1];
        }
        printf("%e %e\n", r, x[1]);
        for (j = 0; j < x_points; ++j)
        {
            if (model_cb(x, (void *)(&r)) > eps_abs)
            {
                printf("%e %e\n", r, x[1]);
            }
            x[0] = x[1];
        }
    }

    return retval;
}

double model_cb(double x[], void *params)
{
    double r = ((double *)params)[0];

    x[1] = 4.0 * r * x[0] * (1.0 - x[0]);

    return fabs(x[0] - x[1]);
}
/* "a:f:hp:r:t:vA:B:C:D:P:T:" */
void print_usage()
{
    printf("OVERVIEW: Holling-Tanner predator-prey model simulation.\n\n");
    printf("USAGE: holling-tanner [options]\n\n");
    printf("OPTIONS:\n");
    printf("  -a <error>     Absolute error. Default is %e\n", OPTION_DEFAULT_AERROR);
    printf("  -f <file>      Output file. Default is " OPTION_DEFAULT_FILE "\n");
    printf("  -h             Print this message\n");
    printf("  -r <value>     Number of \'r\' values. Default is %d\n", OPTION_DEFAULT_R_POINTS);
    printf("  -v             Verbose mode\n");
    printf("  -w <value>     Number of \'x\' values. Default is %d\n", OPTION_DEFAULT_X_POINTS);
}
