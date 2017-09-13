#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
//#include <boost/algorithm/string.hpp>
#include "naive_bayes.h"

using namespace std;

NaiveBayes::NaiveBayes (const char *field_file)
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
    string line;
    ifstream ifs(train_file);

    while (getline(ifs, line)) {
        size_t space = line.find(" ");
        const string &label = line.substr(0, space);
        //Json json = Json::parse(line.substr(++space));
        Json json = Json::parse(line.substr(++space))["_source"].flatten();
        filterFields(json);
        for (auto itr = json.begin(); itr != json.end(); ++itr) {
            //cout << itr.key() << ": " << itr.value() << endl;
            _labels[label][*itr]++;
            _events[*itr][label]++;
        }
    }

    // Post process the total.
    _labels.computeTotal();
    _events.computeTotal();
}

struct scoreCmp {
    bool operator()(const pair<string, double> a, const pair<string, double> b) {
        return a.second > b.second;
    }
};

void NaiveBayes::test (const char *test_file)
{
    int correct = 0, total = 0;
    string line;
    ifstream ifs(test_file);

    while (getline(ifs, line)) {
        size_t space = line.find(" ");
        const string &true_label = line.substr(0, space);
        Json json = Json::parse(line.substr(++space))["_source"].flatten();
        filterFields(json);

        // Compute the scores against each label.
        //vector<pair<string, double> > scores;
        struct { string label; double score; } best_matched = { "", 0 };
        for (auto itr = _labels.begin(); itr != _labels.end(); ++itr) {
            const string &label = itr->first;
            double prob = probability(label, json);
            if (prob >= best_matched.score) {
                best_matched = { label, prob };
            }
            //scores.push_back(make_pair(label, prob));
            //cout << label << ": " << prob << endl;
        }
        // Sort the score in decending order.
        //sort(scores.begin(), scores.end(), scoreCmp());
        //const string &best_match = scores.begin()->first;
        bool matched = true_label == best_matched.label;
        cout << true_label << '\t'
             << best_matched.label << '\t'
             << (matched ? "Correct" : "Wrong  ")
             << "  (" << -log(best_matched.score) << ')'
             << endl;
        if (matched) {
            correct++;
        }
        total++;
    }

    cout << endl
         << "Accuracy: " << correct << " / " << total << " = "
         << setprecision(3) << (float)correct / total * 100 << "%"
         << endl
         << endl;
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
float NaiveBayes::probability (const string &c, const Json &json)
{
    double pc  = (double)_labels[c]["_total"]  / _labels.total();
    double pxi = 1.0;
    for (auto itr = json.begin(); itr != json.end(); ++itr) {
        pxi *= (double)_labels[c][*itr] / _labels[c]["_total"];
    }

    return pxi * pc;
}

void NaiveBayes::filterFields (Json &json)
{
    if (field_table.size() == 0) {
        return;
    }

    Json j;
    for (auto itr = field_table.begin(); itr != field_table.end(); ++itr) {
        auto field = json[itr->first];
        string str_field;
        if (field.is_string()) {
            str_field = field;
        } else if (field.is_number()) {
            str_field = to_string(field.get<int>());
        }
        j[itr->first] = str_field;
    }

    //cout << json.dump(4) << endl;
    json.clear();
    json = j;
    //cout << json.dump(4) << endl;
}
