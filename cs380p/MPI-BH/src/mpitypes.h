#ifndef _MPITYPES_BODY_CELL_H
#define _MPITYPES_BODY_CELL_H

#include <mpi.h>

extern MPI_Datatype mpiBody;

void createMPIBody();

void initializeMPITypes();

void freeMPITypes();

#endif // _MPITYPES_BODY_CELL_H