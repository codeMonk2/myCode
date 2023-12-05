/* input/output */
#include <iostream>

/* MPI library */
#include <mpi.h>
#include "mpitypes.h"
#include "argparse.h"
#include "io.h"
#include "tree.h"
#include <vector>
#include <array>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <cstring>

using namespace std;

array<double, 2> compute_force(const struct options_t *opts, const struct Node *node, const struct Body *b){

    array<double, 2> f;
    double norm[2];
    const double G=.0001;
    //const double G=6.67430e-11;

    if (node && node->b && b->mass != -1) {
        if (node->b->index != b->index) {
            norm[0] = node->b->pos[0]-b->pos[0];
            norm[1] = node->b->pos[1]-b->pos[1];
            double d = sqrt(norm[0] * norm[0] + norm[1] * norm[1]);
            double limitd = d >= 0.03 ? d : 0.03;
            if (!node->divided || ((double)node->s/d < opts->threshold)) {
                for (int i = 0; i < 2; i++) {
                    f[i] = ((G * (node->b->mass * b->mass))/(limitd * limitd)) * (norm[i]/d);
                }
                return f;
            }
        } else {
            return {{0,0}};
        }
    } else {
        return {{0,0}};
    }

    array<double, 2> NEf = {0,0};
    array<double, 2> NWf = {0,0};
    array<double, 2> SEf = {0,0};
    array<double, 2> SWf = {0,0};
    if (node->divided) {
        if (node->chd[0]) {
            NEf = compute_force(opts, node->chd[0], b);
        }
        if (node->chd[1]) {
            NWf = compute_force(opts, node->chd[1], b);
        }
        if (node->chd[2]) {
            SEf = compute_force(opts, node->chd[2], b);
        }
        if (node->chd[3]) {
            SWf = compute_force(opts, node->chd[3], b);
        }
    }
    for (int i = 0; i < 2; i++) {
        f[i] = NEf[i]+NWf[i]+SEf[i]+SWf[i];
    }
    return f;
}

int BHSeq(int argc, char * argv[]) {

    struct options_t opts;
    double dt;
    double start_time, stop_time;
    struct Body *bodies = NULL;
    struct Node *root;
    int s,i,c;

    get_opts(argc, argv, &opts);

    read_file(&opts, &bodies, 1);

    dt = opts.timestep;

    start_time = MPI_Wtime();

    for (s = 0; s < opts.n_steps; s++) {

        root = (struct Node *)malloc(sizeof(struct Node));
        memset((char *)root, 0, sizeof(struct Node));

        root->min_bound[0]=root->min_bound[1]=0.0;
        root->max_bound[0]=root->max_bound[1]=4.0;
        root->s = 4.0;
        root->divided = false;
        root->parent = NULL;
        root->b = NULL;
        root->bIdx = 0;

        for (int i = 0; i < opts.n_bodies; ++i) {
            insertBody(&opts, root, &bodies[i]);
        }

        /* Compute forces */
        std::vector<array<double, 2>> forces(opts.n_bodies, {0,0});
        for(i = 0; i < opts.n_bodies; i++) {
            struct Body *b = &bodies[i];
            array<double, 2> f = compute_force(&opts, root, b);
            forces[i]=f;
        }

        /* Update positions */
        for (i = 0; i < opts.n_bodies; i++) {
            struct Body *b = &bodies[i];
            for(c = 0; c < 2; c++) {
                b->pos[c] = b->pos[c] + b->vel[c] * dt + 0.5 * (forces[i][c]/b->mass) * (dt * dt);
                b->vel[c] = b->vel[c] + (forces[i][c]/b->mass) * dt;
            }
            if (!contains(root, b)) {
                b->mass = -1;
            }
        }

        tearDownTree(root);
    }

    stop_time = MPI_Wtime();

    printf("%lf\n", stop_time-start_time);

    write_file(&opts, bodies);

    if (bodies)
        free(bodies);

    return 0;
}

int BHParallel(int argc, char * argv[], int rank, int size) {

    struct options_t opts;
    double dt;
    double start_time, stop_time;
    struct Body *bodies = NULL;
    struct Node *root;
    int s,i,c;
    struct Body *tempBodies = NULL;

    get_opts(argc, argv, &opts);

    read_file(&opts, &bodies, size);

    dt = opts.timestep;

    int subGrps = opts.n_bodiesParallel/size;

    initializeMPITypes();
    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    for (s = 0; s < opts.n_steps; s++) {

        tempBodies = &bodies[(rank*subGrps)];

        root = (struct Node *)malloc(sizeof(struct Node));
        memset((char *)root, 0, sizeof(struct Node));

        root->min_bound[0]=root->min_bound[1]=0.0;
        root->max_bound[0]=root->max_bound[1]=4.0;
        root->s = 4.0;
        root->divided = false;
        root->parent = NULL;
        root->b = NULL;
        root->bIdx = 0;

        for (int i = 0; i < opts.n_bodies; ++i) {
            insertBody(&opts, root, &bodies[i]);
        }

        /* Compute forces */
        std::vector<array<double, 2>> forces(subGrps, {0,0});
        for(i = 0; i < subGrps; i++) {
            struct Body *b = &tempBodies[i];
            if (b->index == -10) continue;
            array<double, 2> f = compute_force(&opts, root, b);
            forces[i]=f;
        }

        /* Update positions */
        for(i = 0; i < subGrps; i++) {
            struct Body *b = &tempBodies[i];
            if (b->index == -10) continue;
            for(c = 0; c < 2; c++) {
                b->pos[c] = b->pos[c] + b->vel[c] * dt + 0.5 * (forces[i][c]/b->mass) * pow(dt,2);
                b->vel[c] = b->vel[c] + (forces[i][c]/b->mass) * dt;
            }
            if (!contains(root, b)) {
                b->mass = -1;
            }
        }

        MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, bodies, subGrps, mpiBody, MPI_COMM_WORLD);

        tearDownTree(root);
    }

    stop_time = MPI_Wtime();

    if (rank == 0) {
        printf("%lf\n", (stop_time-start_time));
        write_file(&opts, bodies);
    }

    if (bodies)
        free(bodies);

    /* Finalize */
    freeMPITypes();
    MPI_Finalize();

    return 0;
}

int main(int argc, char * argv[]) {

    int size, rank;

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size > 1) {
        return BHParallel(argc, argv, rank, size);
    } else {
        return BHSeq(argc, argv);
    }
}
