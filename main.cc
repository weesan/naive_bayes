/*
 * main.cc - Main program to demo the naive bayes text classifier.
 *
 * Author: WeeSan Lee <weesan@weesan.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include "config.h"
#include "naive_bayes.h"

using namespace std;

static void usage (const char *program)
{
    printf("Usage: %s [-t] [-T]\n", program);
}

int main (int argc, char *argv[])
{
    int opt;
    const char *train_file = NULL;
    const char *test_file = NULL;
    const char *field_map = NULL;
    
    while ((opt = getopt(argc, argv, "t:T:f:")) != EOF) {
        switch (opt) {
        case 't':
            train_file = optarg;
            break;
        case 'T':
            test_file = optarg;
            break;
        case 'f':
            field_map = optarg;
            break;
        default:
            usage(argv[0]);
            exit(1);
        }
    }

    argc -= optind;
    argv += optind;
    
    NaiveBayes nb(field_map);
    nb.train(train_file);
    //nb.dump();
    /*
    cout << "P(Yes|Sunny) = "
         << nb.probability("Yes", string("Sunny"))
         << endl;
    */
    nb.test(test_file);
    
    return 0;
}
