#include "prefix_sum.h"
#include "helpers.h"
#include "operators.h"
#include <math.h>

void upSweep(int blockStart, int blockEnd, struct prefix_sum_args_t *ps_args)
{
    int *values = ps_args->output_vals;
    spin_barrier_t *barrier = ps_args->t_barrier;
    int ai = 0, bi = 0, depth = 0, stride = 1;
    int logSize = logTwo(ps_args->n_vals);
    while(depth<logSize)
    {
	unsigned int cnt = (ps_args->n_vals)>>(depth+1);
        for(unsigned int i = 0; i < cnt; i++)
        {
            ai = stride * (2*i+1) - 1;
            bi = stride * (2*i+2) - 1;
	    int a = values[ai];
	    int b = values[bi];

            // If the index belongs in the range
            // blockStart and blockEnd - update the value
            if ((bi-blockStart)*(bi-blockEnd) <= 0)
            {
                values[bi] = ps_args->op(b, a, ps_args->n_loops);
            }
        }
        depth++;
        stride <<= 1;
        spin_barrier_wait(barrier);
    }
}

void downSweep(int blockStart, int blockEnd, struct prefix_sum_args_t *ps_args)
{
    int *values = ps_args->output_vals;
    spin_barrier_t *barrier = ps_args->t_barrier;
    int ai = 0, bi = 0, depth = logTwo(ps_args->n_vals);
    int stride = 1<<(depth-1);

    while(depth>0)
    {
        unsigned int cnt = (ps_args->n_vals)/(1<<(depth));
        for(unsigned int i = 0; i < cnt; i++)
        {
            ai = stride * (2*i+1)-1;
            bi = stride * (2*i+2)-1;

            {
                int a = values[ai];
                int b = values[bi];
                spin_barrier_wait(barrier);

                // If the index belongs in the range
                // blockStart and blockEnd - update the value
                if ((ai-blockStart)*(ai-blockEnd) <= 0)
                {
                    values[ai] = b;
                }
                if ((bi-blockStart)*(bi-blockEnd) <= 0)
                {
                    values[bi] = ps_args->op(b, a, ps_args->n_loops);
                }
            }
        }
        depth--;
        stride >>= 1;
        spin_barrier_wait(barrier);
    }
}

void* compute_prefix_sum(void *a)
{
    struct prefix_sum_args_t *ps_args = (struct prefix_sum_args_t *)a;
    int blockSize = ceil((float)ps_args->n_vals/ps_args->n_threads);
    int blockStart = ps_args->t_id*blockSize;
    int blockEnd = (blockStart+blockSize)-1;

    upSweep(blockStart, blockEnd, ps_args);

    ps_args->output_vals[ps_args->n_vals - 1] = 0;

    downSweep(blockStart, blockEnd, ps_args);

    return NULL;
}


#if 0
void upSweep(int *values, unsigned int size)
{
    unsigned int ai = 0, bi = 0, depth = 0, stride = 1;
    unsigned  int logSize = logTwo(size);
    while(depth<logSize)
    {
        unsigned int cnt = size/(1<<(depth+1));
        for(unsigned int i = 0; i < cnt; i++)
        {
            ai = stride * (2*i+1) - 1;
            bi = stride * (2*i+2) - 1;

            values[bi] += values[ai];
        }
        depth++;
        stride <<= 1;
    }
}


void downSweep(int *values, unsigned int size)
{
    unsigned int ai = 0, bi = 0, depth = logTwo(size);
    unsigned int stride = 1<<(depth-1);

    while(depth>0)
    {
        unsigned int cnt = size/(1<<(depth));
        for(unsigned int i = 0; i < cnt; i++)
        {
            ai = stride * (2*i+1)-1;
            bi = stride * (2*i+2)-1;
            {
                int t = values[ai];
                values[ai] = values[bi];
                values[bi] += t;
            }
        }
        depth--;
        stride >>= 1;
    }
}
void* compute_prefix_sum(void *a)
{
    struct prefix_sum_args_t *ps_args = (struct prefix_sum_args_t *)a;
    int tid = ps_args->t_id;
    unsigned int blockSize = ceil((float)ps_args->n_vals/ps_args->n_threads);
    unsigned int blockStart = ps_args->t_id*blockSize;
    int pivotSum = 0;

    upSweep(&ps_args->output_vals[blockStart], blockSize);

    //printf("Calling 1 in tid %d\n", tid);
    spin_barrier_wait(ps_args->t_barrier, tid, 1);

    for (int i = 0; i < tid; i++)
    {
        pivotSum += ps_args->output_vals[(blockSize*(i+1))-1];
    }

    //printf("Calling 2 in tid %d\n", tid);
    spin_barrier_wait(ps_args->t_barrier, tid, 2);

    ps_args->output_vals[(blockSize*(tid+1))-1] = pivotSum;

    downSweep(&ps_args->output_vals[blockStart], blockSize);

    return NULL;
}

void* compute_prefix_sum(void *a)
{
    struct prefix_sum_args_t *ps_args = (struct prefix_sum_args_t *)a;
    int tid = ps_args->t_id;
    int blockSize = ceil((float)ps_args->n_vals/ps_args->n_threads);
    int blockStart = ps_args->t_id*blockSize;
    int stride = 1, ai = 0, bi = 0, depth = 1;
    int maxVals = ps_args->n_vals;

    while (stride < blockSize)
    {
        for(int i = 0; i < blockSize>>depth; i++)
        {
            ai = stride * (2*i+1) - 1;
            bi = stride * (2*i+2) - 1;

            if ((blockStart+ai) < maxVals && (blockStart+bi) < maxVals)
                ps_args->output_vals[blockStart+bi] += ps_args->output_vals[blockStart+ai];
        }
        stride <<= 1;
        depth++;
    }

    spin_barrier_wait(ps_args->t_barrier);

    int tmp = 0;
    for (int i = 1; i <= tid; i++)
    {
        if (((blockSize*(i))-1) < maxVals)
            tmp += ps_args->output_vals[(blockSize*(i))-1];
    }
    spin_barrier_wait(ps_args->t_barrier);

    if (((blockSize*(tid+1))-1) < maxVals)
        ps_args->output_vals[(blockSize*(tid+1))-1] = tmp;

    stride >>= 1;
    depth = 1;
    while (stride > 0)
    {
        for(int i = 0; i<depth; i++)
        {
            int ai = stride*(2*i+1)-1;
            int bi = stride*(2*i+2)-1;
            if ((blockStart+ai) < maxVals && (blockStart+bi) < maxVals)
            {
                int t = ps_args->output_vals[blockStart+ai];
                ps_args->output_vals[blockStart+ai] = ps_args->output_vals[blockStart+bi];
                ps_args->output_vals[blockStart+bi] += t;
            }
        }
        depth *= 2;
        stride >>= 1;
    }
    spin_barrier_wait(ps_args->t_barrier);
    return NULL;
}


void* compute_prefix_sum(void *a)
{
    struct prefix_sum_args_t *ps_args = (struct prefix_sum_args_t *)a;
    int tid = ps_args->t_id;
    int stride = 1;

    ps_args->output_vals[2*tid] = ps_args->input_vals[2*tid];
    ps_args->output_vals[2*tid+1] = ps_args->input_vals[(2*tid)+1];

    for(int d=ps_args->n_vals>>1; d > 0; d >>= 1)
    {
        spin_barrier_wait(ps_args->t_barrier);
        int ai =0, bi = 0;
        if (tid < d)
        {
            ai = stride * (2*tid+1) - 1;
            bi = stride * (2*tid+2) - 1;

            ps_args->output_vals[bi] += ps_args->output_vals[ai];
        }
        stride *= 2;
    }

    if (tid == 0)
    {
        ps_args->output_vals[ps_args->n_vals - 1] = 0;
    }

    for(int d = 1; d < ps_args->n_vals; d *= 2)
    {
        stride >>= 1;
        spin_barrier_wait(ps_args->t_barrier);

        if (tid < d)
        {
            int ai = stride*(2*tid+1)-1;
            int bi = stride*(2*tid+2)-1;

            int t = ps_args->output_vals[ai];
            ps_args->output_vals[ai] = ps_args->output_vals[bi];
            ps_args->output_vals[bi] += t;
        }
    }

    //spin_barrier_wait(ps_args->t_barrier);
    return NULL;
}
#endif
