/*
 * main.cc - Main program to demo the naive bayes text classifier.
 *
 * Author: WeeSan Lee <weesan@weesan.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include <glog/logging.h>
#include "config.h"
#include "naive_bayes.h"

using namespace std;

static void usage (const char *program)
{
    printf("Usage: %s [-t] [-T] [-f] [-l]\n", program);
}

int main (int argc, char *argv[])
{
    int opt;
    const char *program = argv[0];
    const char *train_file = NULL;
    const char *test_file = NULL;
    const char *field_map = NULL;
    const char *label_field = NULL;
    int parallel = 1;
    int best_matched = 1;

    FLAGS_log_prefix = 0;
    FLAGS_logtostderr = 1;
    google::InitGoogleLogging(program);
    
    while ((opt = getopt(argc, argv, "t:T:f:l:P:b:")) != EOF) {
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
        case 'l':
            label_field = optarg;
            break;
        case 'P':
            parallel = atoi(optarg);
            break;
        case 'b':
            best_matched = atoi(optarg);
            break;
        default:
            usage(argv[0]);
            exit(1);
        }
    }

    if (!label_field) {
        cout << "Missing the -l option!" << endl;
        return -1;
    }

    argc -= optind;
    argv += optind;

    NaiveBayes nb(field_map, parallel);
    nb.train(train_file, label_field);
    //nb.dump();
    /*
    cout << "P(Yes|Sunny) = "
         << nb.probability("Yes", string("Sunny"))
         << endl;
    */
    nb.test(test_file, label_field, best_matched);

    return 0;
}
