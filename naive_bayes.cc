#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <boost/algorithm/string.hpp>
#include "naive_bayes.h"
#include "test.h"
#include "train.h"

using namespace std;

NaiveBayes::NaiveBayes (const char *field_file,
                        const char *label_field,
                        int parallel,
                        InputFormat input_format) :
    _label_field(label_field),
    _parallel(parallel),
    _input_format(input_format)
{
    if (!field_file) {
        return;
    }

    // Construct the field table from the field file.
    string line;
    ifstream ifs(field_file);
    
    while (getline(ifs, line)) {
        if (line[0] == '#' || line[0] == ';') {
            continue;
        }
        field_table[line] = true;
    }
}

void NaiveBayes::train (const char *train_file)
{
    Train(*this, train_file, _label_field, _parallel);
}

void NaiveBayes::test (const char *test_file, int best_matched)
{
    Test(*this, test_file, _label_field, _parallel, best_matched);
}

void NaiveBayes::classify (const string &unknown, const string &label_field,
                           string &true_label, TopQueue &top_queue)
{
    vector<string> fields;
        
    //json = Json::parse(unknown)["_source"].flatten();
    //true_label = json[string("/") + label_field];
    switch (_input_format) {
    case JSON: {
        Json json = Json::parse(unknown).flatten();
        true_label = json[label_field];
        extract_fields(json, fields);
        break;
    }
    case CSV: {
        vector<string> tokens;
        boost::split(tokens, unknown, boost::is_any_of(","));
        true_label = tokens[atoi(label_field.c_str())];
        extract_fields(tokens, fields);
        break;
    }
    }

    // Compute the scores against each label.
    for (auto itr = _labels.begin(); itr != _labels.end(); ++itr) {
        const string &label = itr->first;
        double prob = log_probability(label, fields);
        
        //cout << label << ": " << prob << endl;
        top_queue.push(Prediction(label, prob));
    }
}

/*
 *          P(x|c) * P(c)
 * P(c|x) = -------------
 *              P(x)
 *
 *
 * Eg.
 *                P(Sunny|Yes) * P(Yes)
 * P(Yes|Sunny) = ---------------------
 *                      P(Sunny)
 *
 * Where Yes and No are the labels, the rest are terms.
 */
float NaiveBayes::probability (const string &c, const string &x)
{
    double pxc = (double)_labels[c][x]         / _labels[c]["_total"];
    double pc  = (double)_labels[c]["_total"]  / _labels.total();
    double px  = (double)_events[x]["_total"]  / _events.total();
    
    return pxc * pc / px;
}

/*
 * P(c|Xi) = P(X1|c) * P(X2|c) * ... * P(Xi|c) * P(c)
 */
float NaiveBayes::probability (const string &c, const vector<string> &fields)
{
    double c_total = (double)_labels[c]["_total"];
    double pc  = c_total / _labels.total();
    double pxi = pc;
    for (auto itr = fields.begin(); itr != fields.end(); ++itr) {
        //pxi *= (double)_labels[c][*itr] / c_total;
        if (_labels.find(c) != _labels.end() &&
            _labels[c].find(*itr) != _labels[c].end()) {
            pxi *= (double)_labels[c][*itr] / c_total;
        } else {
            pxi = 1E-99;
            break;
        }
    }

    return pxi;
}

float NaiveBayes::log_probability (const string &c, const vector<string> &fields)
{
    double c_total = log(_labels[c]["_total"]);
    double pc  = c_total - log(_labels.total());
    double pxi = 0;
    for (auto itr = fields.begin(); itr != fields.end(); ++itr) {
        if (_labels.find(c) != _labels.end() &&
            _labels[c].find(*itr) != _labels[c].end()) {
            pxi += log(_labels[c][*itr]) - c_total;
        } else {
            pxi = log(1E-99);
            break;
        }
    }

    return pxi;
}

void NaiveBayes::extract_fields (Json &json, vector<string> &fields)
{
    if (field_table.size() == 0) {
        return;
    }

    for (auto itr = field_table.begin(); itr != field_table.end(); ++itr) {
        auto field = json[itr->first];
        string str_field;
        if (field.is_string()) {
            str_field = field;
        } else if (field.is_number()) {
            str_field = to_string(field.get<int>());
        }

        // Split the string into words.
        vector<string> tokens;
        boost::split(tokens, str_field, boost::is_any_of(" ,"));
        copy(tokens.begin(), tokens.end(), back_inserter(fields));
    }

    //cout << json.dump(4) << endl;
}

void NaiveBayes::extract_fields (const vector<string> &tokens,
                                vector<string> &fields)
{
    if (field_table.size() == 0) {
        return;
    }

    for (auto itr = field_table.begin(); itr != field_table.end(); ++itr) {
        fields.push_back(tokens[atoi(itr->first.c_str())]);
    }
}
