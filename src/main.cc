/*
 * main.cc - Main program to demo the naive bayes text classifier.
 *
 * Author: WeeSan Lee <weesan@weesan.com>
 */

#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>
#include <glog/logging.h>
#include "config.h"
#include "naive_bayes.h"

using namespace std;

static void usage (const char *program)
{
    cerr << "Usage: " << program << " [-bcCfhJlPtT]" << endl
         << endl
         << "\t-b\tBest/Top matched." << endl
         << "\t-c\tConfidence threshold." << endl
         << "\t-C\tCSV format." << endl
         << "\t-f\tField map." << endl
         << "\t-h\tHelp." << endl
         << "\t-J\tJSON format." << endl
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
    vector<string> train_files;
    const char *test_file = NULL;
    const char *field_map = NULL;
    const char *label_field = NULL;
    int parallel = 1;
    int best_matched = 1;
    bool csv_format = false;
    bool json_format = false;
    NaiveBayes::InputFormat input_format = NaiveBayes::UNKNOWN;
    float confidence = log(1E-99);

    FLAGS_log_prefix = 0;
    FLAGS_logtostderr = 1;
    google::InitGoogleLogging(program);
    
    while ((opt = getopt(argc, argv, "b:c:Cf:hJl:P:t:T:")) != EOF) {
        switch (opt) {
        case 'b':
            best_matched = atoi(optarg);
            break;
        case 'c':
            confidence = atof(optarg);
            break;
        case 'C':
            csv_format = true;
            break;
        case 'f':
            field_map = optarg;
            break;
        case 'J':
            json_format = true;
            break;
        case 'l':
            label_field = optarg;
            break;
        case 'P':
            parallel = atoi(optarg);
            break;
        case 't':
            train_files.push_back(optarg);
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

    if (train_files.empty()) {
        cerr << "Warning: missing the -t option!" << endl;
        return -1;
    }
    
    if (!test_file) {
        cerr << "Warning: missing the -T option!" << endl;
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

    NaiveBayes nb(field_map, label_field, parallel, input_format);
    if (!train_files.empty()) {
        nb.train(train_files);
    }
    if (test_file) {
        nb.test(test_file, confidence, best_matched);
    }

    return 0;
}
