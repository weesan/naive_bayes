#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include "json.hpp"

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
    
public:
    NaiveBayes(const char *field_file);
    void train(const char *train_file);
    void test(const char *test_file);
    float probability(const string &c, const string &x);
    float probability(const string &c, const Json &json);
    void dump(void) {
        cout << "=> Labels (" << _labels.size() << "):" << endl;
        _labels.dump();
        cout << "=> Events (" << _events.size() << "):" << endl;
        _events.dump();
    }

private:
    void filterFields(Json &json);
};

#endif // NAIVE_BAYES_H
