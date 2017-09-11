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
            _bagOfWords[label][word]++;
        }
    }

    // Post process the total.
    _bagOfWords.countTotalLabels();

    printf("Total labels: %d, total unique labels: %d\n",
           _bagOfWords.totalLabels(), _bagOfWords.totalUniqueLabels());
}

/*
 *          P(x|c) * P(c)
 * P(c|x) = -------------
 *              P(x)
 */
float NaiveBayes::probability(const string &c, const string &x)
{
    float pc = (float)_bagOfWords[c].size() / _bagOfWords.totalLabels();
    //float px = (float)_bagOfWords[
}
