#include "tree.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

void printTree(struct Node *node)
{
    if (node) {
        printf("===================================================\n");
        printf("s=%lf min=(%le,%le) max=(%le,%le) div=%d\n",
                node->s, node->min_bound[0], node->min_bound[1],
                node->max_bound[0], node->max_bound[1],node->divided);
        if (node->b) {
            printf("mass=%le (x,y)=(%le,%le) Idx=%d\n",
                   node->b->mass, node->b->pos[0],
                   node->b->pos[1], node->bIdx);
        }
        printf("===================================================\n");
        printTree(node->chd[0]);
        printTree(node->chd[1]);
        printTree(node->chd[2]);
        printTree(node->chd[3]);
    }
}

void tearDownTree(struct Node *node)
{
    if (node) {
        tearDownTree(node->chd[0]);
        tearDownTree(node->chd[1]);
        tearDownTree(node->chd[2]);
        tearDownTree(node->chd[3]);
        if (node->divided && node->b)
            free(node->b);
        free(node);
    }
}

bool contains(struct Node *node, struct Body *b)
{
    return ((b->pos[0] >= node->min_bound[0] and b->pos[0] <= node->max_bound[0]) and
            (b->pos[1] >= node->min_bound[1] and b->pos[1] <= node->max_bound[1]));
}

void insertBody(struct options_t *opts, struct Node *node, struct Body *b) {
    if (!node or !b) return;

    if (node->b == NULL and !node->divided) {
        node->b = b;
        node->bIdx = b->index;
    } else {
        // Node already contains a body, but is not yet split
        if (!node->divided) {
            double xmin = node->min_bound[0], ymin=node->min_bound[1];
            double xmax = node->max_bound[0], ymax=node->max_bound[1];
            double minB[2], maxB[2];
            struct Body *temp = node->b;

            for (int i = 0; i < 4; i++) {
                switch(i) {
                    case 0: // NE
                    {
                        minB[0]=(double)(xmin+(xmax-xmin)/2);
                        minB[1]=(double)(ymin+(ymax-ymin)/2);
                        maxB[0]=xmax;
                        maxB[1]=ymax;
                        break;
                    }
                    case 1: // NW
                    {
                        minB[0]=xmin;
                        minB[1]=(double)(ymin+(ymax-ymin)/2);
                        maxB[0]=(double)(xmin+(xmax-xmin)/2);
                        maxB[1]=ymax;
                        break;
                    }
                    case 2: // SE
                    {
                        minB[0]=(double)(xmin+(xmax-xmin)/2);
                        minB[1]=ymin;
                        maxB[0]=xmax;
                        maxB[1]=(double)(ymin+(ymax-ymin)/2);
                        break;
                    }
                    case 3: // SW
                    {
                        minB[0]=xmin;
                        minB[1]=ymin;
                        maxB[0]=(double)(xmin+(xmax-xmin)/2);
                        maxB[1]=(double)(ymin+(ymax-ymin)/2);
                        break;
                    }
                }

                node->chd[i] = (struct Node *)malloc(sizeof(struct Node));
                memset((char *)node->chd[i], 0, sizeof(struct Node));
                node->chd[i]->min_bound[0]=minB[0];
                node->chd[i]->min_bound[1]=minB[1];
                node->chd[i]->max_bound[0]=maxB[0];
                node->chd[i]->max_bound[1]=maxB[1];
                node->chd[i]->divided = false;
                node->chd[i]->parent = node;
                node->chd[i]->s = node->s/2;

                if ((node->b == temp) and contains(node->chd[i], node->b)) {

                    insertBody(opts, node->chd[i], node->b);

                    // create a pseudobody in the node
                    temp = (struct Body *)malloc(sizeof(struct Body));
                    memset((char *)temp, 0, sizeof(struct Body));
                    temp->pos[0] = node->b->pos[0];
                    temp->pos[1] = node->b->pos[1];
                    temp->mass = node->b->mass;
                    temp->index = -1;

                    node->b = temp;
                    node->bIdx = -1;
                    node->divided = true;
                }
            }
        }

        for(int i = 0; i < 4; i++) {
            if (node->chd[i] and contains(node->chd[i], b)) {
                insertBody(opts, node->chd[i], b);
                break;
            }
        }

        for(int c = 0; c < 2; c++) {
            node->b->pos[c] = ((node->b->mass * node->b->pos[c]) + (b->mass * b->pos[c]))/(node->b->mass+b->mass);
        }
        node->b->mass += b->mass;
    }
}
