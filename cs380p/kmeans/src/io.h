#ifndef _IO_H
#define _IO_H

#include <argparse.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

void read_file(struct options_t* args,
               double **inputData);

//void write_file(struct options_t*         args,
//                struct prefix_sum_args_t* opts,
//                bool   sequential);

#endif
