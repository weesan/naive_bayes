#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;

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
    
public:
    void train(const char *train_file);
    float probability(const string &c, const string &x);
    void dump(void) {
        cout << "=> Labels:" << endl;
        _labels.dump();
        cout << "=> Events:" << endl;
        _events.dump();
    }
};

#endif // NAIVE_BAYES_H
