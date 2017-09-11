#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "naive_bayes.h"

using namespace std;

void NaiveBayes::train (const char *train_file)
{
    string line;
    vector<string> words;
    ifstream ifs(train_file);

    while (getline(ifs, line)) {
        boost::split(words, line,
                     boost::is_any_of(" ,\n"), boost::token_compress_on);
        const string &label = words[0];
        for (int i = 1; i < words.size(); i++) {
            const string &word = words[i];
            _labels[label][word]++;
            _events[word][label]++;
        }
    }

    // Post process the total.
    _labels.computeTotal();
    _events.computeTotal();
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
