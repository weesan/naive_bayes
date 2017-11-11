#ifndef NAIVE_BAYES_H
#define NAIVE_BAYES_H

#include <string>
#include <vector>
#include "field_table.h"
#include "freq_table.h"
#include "top_queue.h"
#include "thread/src/thread.h"

using namespace std;

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
    FieldTable _field_table;
    string _label_field;
    int _parallel;
    InputFormat _input_format;
    
public:
    NaiveBayes(const char *field_file,
               const char *label_field,
               int parallel,
               InputFormat input_format) :
        _field_table(field_file),
        _label_field(string("/") + label_field),
        _parallel(parallel),
        _input_format(input_format) {
    }
    FieldTable &field_table(void) {
        return _field_table;
    }
    InputFormat input_format(void) const {
        return _input_format;
    }
    const string &label_field(void) const {
        return _label_field;
    }
    int parallel(void) const {
        return _parallel;
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
        //_labels.dump();
    }
    void train(const vector<string> train_files);
    void test(const char *test_file, float confidence, int best_matched = 1);
    void classify(const string &unknown, const string &label_field,
                  string &true_label, TopQueue &top_queue);
    float probability(const string &c, const string &x);
    float probability(const string &c, const vector<string> &fields);
    float log_probability(const string &c, const vector<string> &fields);
    void dump(void);
};

#endif // NAIVE_BAYES_H
