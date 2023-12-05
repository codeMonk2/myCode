#ifndef _IO_H
#define _IO_H

#include <mpi.h>
#include "tree.h"
#include <iostream>
#include <fstream>
#include <vector>

void read_file(struct options_t* args,
               struct Body **bodies, int size);

void write_file(struct options_t* args,
                struct Body *bodies);

#endif