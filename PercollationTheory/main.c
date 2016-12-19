#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#define __USE_POSIX2           (1)
#endif

#include <unistd.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#define UNUSED(x) (void)(x)

#define DEFAULT_STEP            (1e-5)
#define MAX_STRING_SIZE         (4096)
#define OPTIONS                 "f:hp:s:vL:"

#define OPTION_DEFAULT_FILE     "data.dat"

#define OPTION_DEFAULT_GRID_SIZE        (size_t)(100)
#define OPTION_DEFAULT_FILL_PROBABILTY          (0.5)
#define OPTIOND_DEFAULT_RANDOM_SEED             (0)

void print_usage();

int fill_grid(gsl_matrix * matrix, double fill_probability);
int hoshen_kopelman(gsl_matrix * matrix);
int save_matrix(gsl_matrix * matrix);
int check_percollation(gsl_matrix * matrix);
int hosheen_kopelman_merged(gsl_matrix * matrix, double fill_probability);

int main(int argc, char *const * argv)
{
    int retval = GSL_SUCCESS;
    int percollation_cluster_label = -1;
    char option = 0;
    int verbose = 0;
    int force_save = 0;
    char file_name[MAX_STRING_SIZE] = OPTION_DEFAULT_FILE;

    gsl_matrix * matrix = NULL;

    double p = OPTION_DEFAULT_FILL_PROBABILTY;
    double step = DEFAULT_STEP;
    size_t L = OPTION_DEFAULT_GRID_SIZE;

    unsigned int seed = OPTIOND_DEFAULT_RANDOM_SEED;

    while ((option = getopt(argc, argv, OPTIONS)) != -1)
    {
        switch (option)
        {
            case 'f':
                strcpy(file_name, optarg);
            break;
            case 'h':
                print_usage();
                retval = GSL_SUCCESS;
                goto done;
            break;
            case 'p':
                if (1 != sscanf(optarg, "%le", &p))
                {
                    fprintf(stderr, "Error: bad fill probability value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
                if (p > 1 || p < 0)
                {
                    fprintf(stderr, "Error: bad fill probability value. Should be in [0:1].\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 's':
                if (1 != sscanf(optarg, "%u", &seed))
                {
                    fprintf(stderr, "Error: bad random seed value. Should be number.\n");
                    retval = GSL_ERANGE;
                    goto done;
                }
            break;
            case 'v':
                verbose = 1;
            break;
            case 'F':
                force_save = 1;
            break;
            case 'L':
                if (1 != sscanf(optarg, "%lu", &L))
                {
                    fprintf(stderr, "Error: bad grid size value. Should be number.\n");
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
        printf("# Fill probability:                       %f\n", p);
        printf("# Grid size:                              %lu\n", L);
        printf("# Random seed:                            %u\n", seed);
    }

    if (stdout != freopen(file_name, "w", stdout))
    {
        fprintf(stderr, "Error: could not open file %s", file_name);
        retval = GSL_FAILURE;
        goto done;
    }

    matrix = gsl_matrix_alloc(L, L);
    if (force_save)
    {
        srand(seed);
        gsl_matrix_set_all(matrix, 0);
        fill_grid(matrix, p);
        hoshen_kopelman(matrix);
        save_matrix(matrix);

        goto done;
    }

    for (; p <= 1; p += step)
    {
        srand(seed);
        hosheen_kopelman_merged(matrix, p);
        percollation_cluster_label = check_percollation(matrix);

        if (percollation_cluster_label > 0)
        {
            break;
        }
    }

    save_matrix(matrix);
    fprintf(stderr, "Percollation cluster number: %d\n",
            percollation_cluster_label);
    fprintf(stderr, "Percollation limit: %1.5f\n", p);

done:
    gsl_matrix_free(matrix);
    return retval;
}

int check_percollation(gsl_matrix * matrix)
{
    int retval = -1;
    int * labels = NULL;
    size_t i, k, j, L;

    if (matrix == NULL)
    {
        goto done;
    }

    L = matrix->size1;
    labels = malloc(sizeof(int) * L);

    for (i = 0, j = 0; i < L; ++i)
    {
        int label = gsl_matrix_get(matrix, 0, i);
        if (label != 0)
        {
            if (j != 0)
            {
                if (labels[j - 1] == label)
                {
                    continue;
                }
            }
            labels[j++] = label;
        }
        else
        {
            i += 1;
        }
    }

    for (i = 0; i < L; ++i)
    {
        int label = gsl_matrix_get(matrix, L - 1, i);
        for (k = 0; k < j; ++k)
        {
            if (labels[k] == label)
            {
                retval = label;
                goto done;
            }
        }
    }
    free(labels);
done:
    return retval;
}

int fill_grid(gsl_matrix * matrix, double fill_probability)
{
    int retval = GSL_SUCCESS;
    size_t i, j, L;
    if (matrix == NULL)
    {
        retval = GSL_FAILURE;
        goto done;
    }

    if (matrix->size1 != matrix->size2)
    {
        fprintf(stderr, "Error: matrix should be square.\n");
        retval = GSL_ERANGE;
        goto done;
    }

    L = matrix->size1;

    for (i = 0; i < L; ++i)
    {
        for (j = 0; j < L; ++j)
        {
            double probability = (double)(rand()) / (double)(RAND_MAX);
            if (probability < fill_probability)
            {
                gsl_matrix_set(matrix, i, j, 1);
            }
            else
            {
                gsl_matrix_set(matrix, i, j, 0);
            }
        }
    }

done:
    return retval;
}

static int hk_find(gsl_vector * labels, int x)
{
    int y = x;
    while (gsl_vector_get(labels, y) != y)
    {
        y = gsl_vector_get(labels, y);
    }
    while (gsl_vector_get(labels, x) != x)
    {
        int z = gsl_vector_get(labels, x);
        gsl_vector_set(labels, x, y);
        x = z;
    }
    return y;
}

static int hk_union(gsl_vector * labels, int x, int y)
{
    int value = hk_find(labels, y);
    gsl_vector_set(labels, hk_find(labels, x), hk_find(labels, y));
    return value;
}

int hoshen_kopelman(gsl_matrix * matrix)
{
    int retval = GSL_SUCCESS;
    size_t i, j, L, n, label = 0;

    gsl_vector * labels  = NULL;

    if (matrix == NULL)
    {
        retval = GSL_FAILURE;
        goto done;
    }

    if (matrix->size1 != matrix->size2)
    {
        fprintf(stderr, "Error: matrix should be square.\n");
        retval = GSL_ERANGE;
        goto done;
    }

    L = matrix->size1;

    labels = gsl_vector_alloc(L * L + 1);
    gsl_vector_set_all(labels, 0);
    n = labels->size;

    for (i = 0; i < L; ++i)
    {
        for (j = 0; j < L; ++j)
        {
            int up = 0, left = 0;
            if (gsl_matrix_get(matrix, i, j) == 0)
            {
                continue;
            }

            up = (i == 0 ? 0 : gsl_matrix_get(matrix, i - 1, j));
            left = (j == 0 ? 0 : gsl_matrix_get(matrix, i, j - 1));

            switch (!!up + !!left)
            {
                case 0:
                    label = gsl_vector_get(labels, 0);
                    gsl_vector_set(labels, 0, ++label);
                    gsl_vector_set(labels, label, label);
                    gsl_matrix_set(matrix, i, j, label);
                break;
                case 1:
                    gsl_matrix_set(matrix, i, j, (up > left ? up : left));
                break;
                case 2:
                    gsl_matrix_set(matrix, i, j, hk_union(labels, up, left));
                break;
            }
        }
    }

    for (i = 0; i < L; ++i)
    {
        for (j = 0; j < L; ++j)
        {
            if (gsl_matrix_get(matrix, i, j) == 0)
            {
                continue;
            }

            gsl_matrix_set(matrix, i, j,
                hk_find(labels, gsl_matrix_get(matrix, i, j)));
        }
    }
    gsl_vector_free(labels);
done:
    return retval;
}

int save_matrix(gsl_matrix * matrix)
{
    int retval = GSL_SUCCESS;
    size_t i, j, L;
    if (matrix == NULL)
    {
        retval = GSL_FAILURE;
        goto done;
    }

    if (matrix->size1 != matrix->size2)
    {
        fprintf(stderr, "Error: matrix should be square.\n");
        retval = GSL_ERANGE;
        goto done;
    }

    L = matrix->size1;

    for (i = 0; i < L; ++i)
    {
        for (j = 0; j < L; ++j)
        {
            printf("%2d ", (int)gsl_matrix_get(matrix, i, j));
        }
        printf("\n");
    }
done:
    return retval;
}

int hosheen_kopelman_merged(gsl_matrix * matrix, double fill_probability)
{
int retval = GSL_SUCCESS;
    size_t i, j, L, n, label = 0;

    gsl_vector * labels  = NULL;

    if (matrix == NULL)
    {
        retval = GSL_FAILURE;
        goto done;
    }

    if (matrix->size1 != matrix->size2)
    {
        fprintf(stderr, "Error: matrix should be square.\n");
        retval = GSL_ERANGE;
        goto done;
    }

    L = matrix->size1;

    labels = gsl_vector_alloc(L * L + 1);
    gsl_vector_set_all(labels, 0);
    n = labels->size;

    for (i = 0; i < L; ++i)
    {
        for (j = 0; j < L; ++j)
        {
            int up = 0, left = 0;

            double probability = (double)(rand()) / (double)(RAND_MAX);
            if (probability < fill_probability)
            {
                gsl_matrix_set(matrix, i, j, 1);
            }
            else
            {
                gsl_matrix_set(matrix, i, j, 0);
                continue;
            }

            up = (i == 0 ? 0 : gsl_matrix_get(matrix, i - 1, j));
            left = (j == 0 ? 0 : gsl_matrix_get(matrix, i, j - 1));

            switch (!!up + !!left)
            {
                case 0:
                    label = gsl_vector_get(labels, 0);
                    gsl_vector_set(labels, 0, ++label);
                    gsl_vector_set(labels, label, label);
                    gsl_matrix_set(matrix, i, j, label);
                break;
                case 1:
                    gsl_matrix_set(matrix, i, j, (up > left ? up : left));
                break;
                case 2:
                    gsl_matrix_set(matrix, i, j, hk_union(labels, up, left));
                break;
            }
        }
    }

    for (i = 0; i < L; ++i)
    {
        for (j = 0; j < L; ++j)
        {
            if (gsl_matrix_get(matrix, i, j) == 0)
            {
                continue;
            }

            gsl_matrix_set(matrix, i, j,
                hk_find(labels, gsl_matrix_get(matrix, i, j)));
        }
    }
    gsl_vector_free(labels);
done:
    return retval;
}

/* "f:hp:vL:" */
void print_usage()
{
    printf("OVERVIEW: Percollation theory 2D model.\n\n");
    printf("USAGE: percollation [options]\n\n");
    printf("OPTIONS:\n");
    printf("  -f <file>      Output file. Default is " OPTION_DEFAULT_FILE ".\n");
    printf("  -h             Print this message.\n");
    printf("  -p <value>     Fill probability. Default is %f.\n", OPTION_DEFAULT_FILL_PROBABILTY);
    printf("  -v             Verbose mode.\n");
    printf("  -F             Force save grid without calculating percollation limit.\n");
    printf("  -L <value>     Grid Size. Default is %lu.\n", OPTION_DEFAULT_GRID_SIZE);
}
