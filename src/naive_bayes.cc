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

const double NaiveBayes::CONFIDENCE = log(numeric_limits<double>::min());

void NaiveBayes::train (const vector<string> train_files)
{
    Train(*this, train_files);
}

void NaiveBayes::test (const char *test_file, float confidence, int best_matched)
{
    Test(*this, test_file, confidence, best_matched);
}

void NaiveBayes::classify (const string &unknown, const string &label_field,
                           string &true_label, TopQueue &top_queue)
{
    vector<string> fields;
        
    switch (_input_format) {
    case JSON: {
        Json json = Json::parse(unknown).flatten();
        try {
            true_label = json[label_field];
        } catch (const exception &e) {
            // This may happen when the entry trying to be classfy has
            // no true label.  In which case, we try to get its "id" if
            // available for post analysis purposes.
            try {
                true_label = json["/id"];
            } catch (const exception &e) {
                // As a last resort, let's use empty label.
                true_label = "";
            }
        }
        _field_table.extract(json, fields);
        break;
    }
    case CSV: {
        vector<string> tokens;
        boost::split(tokens, unknown, boost::is_any_of(","));
        true_label = tokens[atoi(label_field.c_str())];
        _field_table.extract(tokens, fields);
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
            pxi = CONFIDENCE;
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
        if (_labels[c].find(*itr) != _labels[c].end()) {
            pxi += log(_labels[c][*itr]) - c_total;
        } else {
            //pxi = CONFIDENCE;
            //break;
            pxi += CONFIDENCE / _field_table.size();
            //pxi += CONFIDENCE;
        }
    }

    return pxi;
}

void NaiveBayes::dump(void) {
    cout << "=> Labels (" << _labels.size() << "):" << endl;
    _labels.dump();
    cout << "=> Events (" << _events.size() << "):" << endl;
    _events.dump();
}
