#ifndef _TREE_H_N_BODY_
#define _TREE_H_N_BODY_

#include "argparse.h"

struct Body {
    int index;
    double pos[2];
    double vel[2];
    double mass;
};

struct Node {
    double min_bound[2];
    double max_bound[2];
    double s;
    bool divided;
    struct Body *b;
    int bIdx;

    struct Node *chd[4];

    struct Node *parent;
};

bool contains(struct Node *node, struct Body *body);
void insertBody(struct options_t *opts, struct Node *node, struct Body *b);
void tearDownTree(struct Node *node);
void printTree(struct Node *node);

#endif //_TREE_H_N_BODY_
