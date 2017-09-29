#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "json.hpp"
#include "freq_table.h"
#include "top_queue.h"
#include "thread/src/thread.h"

using namespace std;
using Json = nlohmann::json;

class NaiveBayes {
public:
    enum InputFormat {
        UNKNOWN,
        JSON,
        CSV
    };
    
private:
    FreqTable _labels, _events;
    Mutex _labels_mutex, _events_mutex;
    unordered_map<string, bool> field_table;
    string _label_field;
    int _parallel;
    InputFormat _input_format;
    
public:
    NaiveBayes(const char *field_file,
               const char *label_field,
               int parallel,
               InputFormat input_format);
    InputFormat input_format(void) const {
        return _input_format;
    }
    void inc_label_count(const string &label, const string &token) {
        _labels_mutex.lock();
        _labels[label][token]++;
        _labels_mutex.unlock();
    }
    void inc_event_count(const string &event, const string &token) {
        _events_mutex.lock();
        _events[event][token]++;
        _events_mutex.unlock();
    }
    void post_process(void) {
        _labels.computeTotal();
        _events.computeTotal();
    }
    void train(const char *train_file);
    void test(const char *test_file, int best_matched = 1);
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
    void extract_fields(Json &json, vector<string> &fields);
    void extract_fields(const vector<string> &tokens, vector<string> &fields);
};

#endif // NAIVE_BAYES_H
