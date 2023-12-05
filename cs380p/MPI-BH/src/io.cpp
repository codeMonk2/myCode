#include "argparse.h"
#include <io.h>
#include <climits>
#include <cstring>
#include <iomanip>

void read_file(struct options_t* opts,
               struct Body **bodies, int size) {

    int subGroupRem, totalBodies;

    // Open file
    std::ifstream in;
    in.open(opts->in_file);
    // Get num vals
    in >> opts->n_bodies;

    if ((opts->n_bodies) <= 0 || (opts->n_bodies) > INT_MAX)
    {
        std::cerr << "Invalid number of inputs in input file " << std::endl;
        exit(1);
    }

    totalBodies = opts->n_bodies;
    if (size > 1) {
        subGroupRem = opts->n_bodies%size;
        totalBodies = opts->n_bodies+((size-subGroupRem)%size);
    }
    opts->n_bodiesParallel = totalBodies;

    int bodySize = totalBodies * sizeof(struct Body);
    *bodies = (struct Body *)malloc(bodySize);
    memset((char *)(*bodies), 0, bodySize);

    // Read input vals
    double min[2], max[2];
    min[0]=min[1]=0;
    max[0]=max[1]=4;
    for (int i = 0; i < opts->n_bodies; ++i) {
        struct Body *b =&((*bodies)[i]);
        in >> b->index;
        in >> b->pos[0];
        in >> b->pos[1];
        in >> b->mass;
        in >> b->vel[0];
        in >> b->vel[1];
        if (b->pos[0] < min[0] or b->pos[1] < min[1])
            b->mass = -1;
        if (b->pos[0] > max[0] or b->pos[0] > max[1])
            b->mass = -1;
    }

    for (int i = opts->n_bodies; i < opts->n_bodiesParallel; i++) {
        struct Body *b =&((*bodies)[i]);
        b->index=-10;
    }
}

void write_file(struct options_t* opts,
                struct Body *bodies) {

    // Open file
    std::ofstream out;
    out.open(opts->out_file, std::ofstream::trunc);

    out << opts->n_bodies << std::endl;

    out << std::scientific;
    out.precision(6);

    // Write solution to output file
    for (int i = 0; i < opts->n_bodies; ++i) {
        struct Body *b = &bodies[i];
        out << b->index << " ";
        out << b->pos[0] << " ";
        out << b->pos[1] << " ";
        out << b->mass << " ";
        out << b->vel[0] << " ";
        out << b->vel[1];
        out << std::endl;
    }

    out.flush();
    out.close();
}