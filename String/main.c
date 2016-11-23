#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef __linux__
#define __USE_POSIX2           (1)
#endif

#define OPTION_DEFAULT_L 1.0
#define OPTION_DEFAULT_A 1.0

#include <unistd.h>

#include <gsl/gsl_integration.h>

#define UNUSED(x) (void)(x)

double phi(double x)
{
    if (x <= OPTION_DEFAULT_L / 10)
    {
        return 50 * ((OPTION_DEFAULT_L / 20) - fabs(x - OPTION_DEFAULT_L / 20));
    }
    else
    {
        return 0;
    }
    if (x == 0 || x == OPTION_DEFAULT_L)
    {
        return 0.;
    }
    return 1.0 / (0.1 * sqrt(2 * M_PI)) * exp(- gsl_pow_2(x - OPTION_DEFAULT_L / 2.0) / (2 * gsl_pow_2(0.1)));
}

double psi(double x)
{
    return 0;
}

double A(double x, void * params)
{
    int n = *(int *)params;
    return (2.0 / OPTION_DEFAULT_L * phi(x) * sin(M_PI * n * x / OPTION_DEFAULT_L));
}

double B(double x, void * params)
{
    int n = *(int *)params;
    return (2.0 / (M_PI * n * OPTION_DEFAULT_A) * psi(x) * sin(M_PI * n * x / OPTION_DEFAULT_L));
}

typedef double (*function)(double x, void * params);

int main(int argc, char *const * argv)
{
    int x, n, t, i;
    int ts = 100;
    int n_max = 20;
    int t_max = 10;
    char buf[100] = "";
    int points = 1000;
    double * u1 = (double *)malloc(sizeof(double) * points * OPTION_DEFAULT_L + 1);
    double * u2 = (double *)malloc(sizeof(double) * points * OPTION_DEFAULT_L + 1);
    double * u3 = (double *)malloc(sizeof(double) * points * OPTION_DEFAULT_L + 1);

    double * u = u1;
    double * u_prev1 = u2;
    double * u_prev2 = u3;
    double r = (OPTION_DEFAULT_A * ((double)t_max) / ts) / (OPTION_DEFAULT_L / points);
    gsl_integration_workspace * w = gsl_integration_workspace_alloc(1000);
    gsl_function f;

    freopen("out/info.dat", "w", stdout);
    printf("%d\n", ts * t_max);

    for (t = 0; t <= t_max * ts; ++t)
    {
        double current_t = ((double)t) / ts;
        sprintf(buf, "out/%d.dat", t);
        freopen(buf, "w", stdout);
        for (x = 0; x <= points * OPTION_DEFAULT_L; ++x)
        {
            double res = 0;
            double x_current = ((double)x) / points;
            for (n = 1; n <= n_max; ++n)
            {
                double buf1 = 0, buf2 = 0, errabs;
                size_t n_calls;
                f.params = &n;
                f.function = &A;
                gsl_integration_qag(&f, 0, OPTION_DEFAULT_L, 1e-3, 1e-3, 1000, 6, w, &buf1,
                                    &errabs);
                buf1 *= cos(M_PI * n * OPTION_DEFAULT_A * current_t / OPTION_DEFAULT_L);
                f.function = &B;
                gsl_integration_qag(&f, 0, OPTION_DEFAULT_L, 1e-3, 1e-3, 1000, 6, w, &buf2,
                                    &errabs);
                buf2 *= sin(M_PI * n * OPTION_DEFAULT_A * current_t / OPTION_DEFAULT_L);
                res += sin(M_PI * n * x_current / OPTION_DEFAULT_L) * (buf1 + buf2);
            }
            printf("%lf %lf\n", ((double)x) / points, res);
        }
    }

    for (i = 0; i <= points * OPTION_DEFAULT_L; ++i)
    {
        double x_current = ((double)i) / points;
        u_prev2[i] = phi(x_current);
        u_prev1[i] = u_prev2[i]  + ((double)t_max) / ts * psi(x_current);
    }
    r = 1;
    fprintf(stderr, "%lf\n", r);
    for (t = 2; t <= t_max * ts; ++t)
    {
        double * buffer;
        sprintf(buf, "out/%d_num.dat", t);
        freopen(buf, "w", stdout);

        printf("%lf %lf\n", 0.0, u[0]);
        for (x = 1; x < points * OPTION_DEFAULT_L; ++x)
        {
            u[x] = 2 * (1 - gsl_pow_2(r)) * u_prev1[x] +
                gsl_pow_2(r) * (u_prev1[x + 1] + u_prev1[x - 1]) -
                u_prev2[x];
            printf("%lf %lf\n", ((double)x) / points, u[x]);
        }
        printf("%lf %lf\n", OPTION_DEFAULT_L, u[(int)(points * OPTION_DEFAULT_L)]);

        buffer = u_prev2;
        u_prev2 = u_prev1;
        u_prev1 = u;
        u = buffer;
    }
    gsl_integration_workspace_free(w);
    return 0;
}
