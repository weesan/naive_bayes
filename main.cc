/*
 * main.cc - Main program to demo the naive bayes text classifier.
 *
 * Author: WeeSan Lee <weesan@weesan.com>
 */

#include <stdlib.h>
#include <iostream>
#include <string>
#include <getopt.h>
#include <glog/logging.h>
#include "config.h"
#include "naive_bayes.h"

using namespace std;

static void usage (const char *program)
{
    cerr << "Usage: " << program << " [-bcfhjlPtT]" << endl
         << endl
         << "\t-b\tBest/Top matched." << endl
         << "\t-c\tCSV format." << endl
         << "\t-f\tField map." << endl
         << "\t-h\tHelp." << endl
         << "\t-j\tJSON format." << endl
         << "\t-l\tLabel field." << endl
         << "\t-P\tParallel processing." << endl
         << "\t-t\tTraining file." << endl
         << "\t-T\tTest file." << endl
         << endl;
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
    bool csv_format = false;
    bool json_format = false;
    NaiveBayes::InputFormat input_format = NaiveBayes::UNKNOWN;

    FLAGS_log_prefix = 0;
    FLAGS_logtostderr = 1;
    google::InitGoogleLogging(program);
    
    while ((opt = getopt(argc, argv, "b:cf:hjl:P:t:T:")) != EOF) {
        switch (opt) {
        case 'b':
            best_matched = atoi(optarg);
            break;
        case 'c':
            csv_format = true;
            break;
        case 'f':
            field_map = optarg;
            break;
        case 'j':
            json_format = true;
            break;
        case 'l':
            label_field = optarg;
            break;
        case 'P':
            parallel = atoi(optarg);
            break;
        case 't':
            train_file = optarg;
            break;
        case 'T':
            test_file = optarg;
            break;
        case 'h':
        default:
            usage(argv[0]);
            exit(1);
        }
    }

    if (!train_file) {
        cerr << "Missing the -t option!" << endl;
        return -1;
    }
    
    if (!test_file) {
        cerr << "Missing the -T option!" << endl;
        return -1;
    }
    
    if (!label_field) {
        cerr << "Missing the -l option!" << endl;
        return -1;
    }

    if (!(csv_format ^ json_format)) {
        cerr << "Missing the either -c or -j option!" << endl;
        return -1;
    }

    if (csv_format) {
        input_format = NaiveBayes::CSV;
    } else if (json_format) {
        input_format = NaiveBayes::JSON;
    }
    
    argc -= optind;
    argv += optind;

    NaiveBayes nb(field_map, parallel, input_format);
    nb.train(train_file, label_field);
    nb.test(test_file, label_field, best_matched);

    return 0;
}
