#include "mpitypes.h"
#include "tree.h"
#include <stddef.h>

MPI_Datatype mpiBody;

void createMPIBody(){
    int blocklengths[4] = {1, 2, 2, 1};
    MPI_Datatype types[4] = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Aint offsets[4];

    offsets[0] = offsetof(struct Body, index);
    offsets[1] = offsetof(struct Body, pos);
    offsets[2] = offsetof(struct Body, vel);
    offsets[3] = offsetof(struct Body, mass);

    MPI_Type_create_struct(4, blocklengths, offsets, types, &mpiBody);
    MPI_Type_commit(&mpiBody);
}

void initializeMPITypes(){
    createMPIBody();
}

void freeMPITypes(){
    MPI_Type_free(&mpiBody);
}