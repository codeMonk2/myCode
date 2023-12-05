#include <argparse.h>

void get_opts(int argc,
              char **argv,
              struct options_t *opts)
{
    if (argc == 1)
    {
    	std::cout << "Usage:" << std::endl;
    	std::cout << "\t-k num_cluster: an integer specifying the number of clusters" << std::endl;
        std::cout << "\t-d dims: an integer specifying the dimension of the points" << std::endl;
        std::cout << "\t-i inputfilename: a string specifying the input filename" << std::endl; 
        std::cout << "\t-m max_num_iter: an integer specifying the maximum number of iterations" << std::endl;
        std::cout << "\t-t threshold: a double specifying the threshold for convergence test." << std::endl;
    	std::cout << "\t-c a flag to control the output of your program." << std::endl;
        std::cout << "\t   If -c is specified, your program should output the centroids of all clusters." << std::endl;
        std::cout << "\t   If -c is not specified, your program should output the labels of all points." << std::endl; 
        std::cout << "\t-s seed: an integer specifying the seed for rand()" << std::endl;
        std::cout << "\t-a type of algorithm used. 1 = seq, 2 = basic cuda, 3 = cuda+shared mem, 4 = cuda+thrust." << std::endl;
        exit(0);
    }

    opts->bPrintCentroids = false;
    opts->threshold=0.00000001;
    opts->randSeed=8675309;

    struct option l_opts[] = {
        {"k", required_argument, NULL, 'k'},
        {"d", required_argument, NULL, 'd'},
        {"i", required_argument, NULL, 'i'},
        {"m", required_argument, NULL, 'm'},
        {"t", required_argument, NULL, 't'},
        {"c", no_argument, NULL, 'c'},
        {"s", required_argument, NULL, 's'},
		{"a", required_argument, NULL, 'a'},
    };

    int ind, c;
    while ((c = getopt_long(argc, argv, "k:d:i:m:t:a:cs:", l_opts, &ind)) != -1)
    {
        switch (c)
        {
        case 0:
            break;
        case 'i':
            opts->in_file = (char *)optarg;
            break;
        case 'k':
            opts->nCentroids = atoi((char *)optarg);
	        break;
        case 'd':
            opts->nDims = atoi((char *)optarg);
	        break;
        case 'm':
            opts->nIters = atoi((char *)optarg);
	        break;
        case 't':
            opts->threshold = atof((char *)optarg);
	        break;
        case 's':
            opts->randSeed = atoll((char *)optarg);
	        break;
        case 'c':
            opts->bPrintCentroids = true;
            break;
        case 'a':
			opts->algoType = atoi((char*)optarg);
            if (!((opts->algoType - 1) * (4 - opts->algoType) >= 0))
			{
				printf("Invalid algoType %d\n", opts->algoType);
				exit(1);
			}
            break;
        case ':':
            std::cerr << argv[0] << ": option -" << (char)optopt << "requires an argument." << std::endl;
            exit(1);
        }
    }
}
