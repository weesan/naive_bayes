/*
 * main.cc - Main program to demo the naive bayes text classifier.
 *
 * Author: WeeSan Lee <weesan@weesan.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <getopt.h>
#include "config.h"
#include "naive_bayes.h"

static void usage (const char *program)
{
    printf("Usage: %s [-t] [-T]\n", program);
}

int main (int argc, char *argv[])
{
    int opt;
    const char *train_file = NULL;
    const char *test_file = NULL;
    
    while ((opt = getopt(argc, argv, "t:T:")) != EOF) {
        switch (opt) {
        case 't':
            train_file = optarg;
            break;
        case 'T':
            break;
            test_file = optarg;
        default:
            usage(argv[0]);
            exit(1);
        }
    }

    argc -= optind;
    argv += optind;
    
    NaiveBayes nb;
    nb.train(train_file);
    nb.dump();

    return 0;
}
