#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "json.hpp"
#include "test.h"
#include "top_queue.h"

using namespace std;
using Json = nlohmann::json;

class FreqTable : public unordered_map<string, unordered_map<string, size_t> > {
private:
    size_t _total;
public:
    FreqTable(void) : _total(0) {
    }
    size_t const total(void) const {
        return _total;
    }
    void computeTotal(void) {
        for (auto i = begin(); i != end(); ++i) {
            size_t total = 0;
            for (auto j = i->second.begin(); j != i->second.end(); ++j) {
                total += j->second;
            }
            (*this)[i->first]["_total"] = total;
            _total += total;
        }
    }
    void dump(void) {
        for (auto i = begin(); i != end(); ++i) {
            cout << i->first << ": ";
            for (auto j = i->second.begin(); j != i->second.end(); ++j) {
                cout << "(" << j->first << ", " << j->second << ") ";
            }
            cout << endl;
        }
    }
};
    
class NaiveBayes {
private:
    FreqTable _labels, _events;
    unordered_map<string, bool> field_table;
    int _parallel;
    
public:
    NaiveBayes(const char *field_file, int parallel);
    const FreqTable &labels (void) const {
        return _labels;
    }
    void train(const char *train_file, const char *label_field);
    void test(const char *test_file, const char *label_field,
              int best_matched = 1);
    void classify(const string &unknown, const string &label_field,
                  string &true_label, TopQueue &top_queue);
    float probability(const string &c, const string &x);
    float probability(const string &c, const vector<string> &fields);
    float log_probability(const string &c, const vector<string> &fields);
    void dump(void) {
        cout << "=> Labels (" << _labels.size() << "):" << endl;
        _labels.dump();
        cout << "=> Events (" << _events.size() << "):" << endl;
        _events.dump();
    }
    void extractFields(Json &json, vector<string> &fields);
};

#endif // NAIVE_BAYES_H
