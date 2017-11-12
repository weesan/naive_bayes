#include <iostream>
#include <fstream>
#include <string>
#include <glog/logging.h>
#include "test.h"
#include "naive_bayes.h"
#include "top_queue.h"

//#define SINGLE_THREADED
#define TEST_MAX_QUEUE_SIZE    1000

using namespace std;

static void classify_helper (Test &test, const string &unknown,
                             float confidence, int best_matched)
{
    TopQueue top_queue(best_matched);
    string true_label = "";
    NaiveBayes &naive_bayes = test.naive_bayes();
    const string &label_field = naive_bayes.label_field();
  
    naive_bayes.classify(unknown, label_field, true_label, top_queue);
        
    bool matched = top_queue.matched(true_label);
    const Prediction &prediction = top_queue.topMatched();
    bool low_confidence = prediction.score() <= NaiveBayes::CONFIDENCE * confidence;

    stringstream ss;
    ss << left
       << setw(16) << true_label << " "
       << setw(16) << prediction.label() << " "
       << setw(9)  << (matched ? "Correct" : "Wrong  ") << " "
       << prediction.score() << " "
       << (low_confidence ? "(low)" : "")
       << endl;
    LOG(INFO) << ss.str();

    if (low_confidence) {
        test.inc_low_confidence();
    } else if (matched) {
        test.inc_correct();
    }
}

class ClassifyTask : public Task {
private:
    Test &_test;
    string _unknown;
    float _confidence;
    int _best_matched;
    
public:
    ClassifyTask(Test &test, const string &unknown,
                 float confidence, int best_matched = 1) :
        _test(test),
        _unknown(unknown),
        _confidence(confidence),
        _best_matched(best_matched) {
    }
    void run(void) {
        //cout << _unknown << endl;
        classify_helper(_test, _unknown, _confidence, _best_matched);
    }
};

Test::Test (NaiveBayes &naive_bayes, const string &test_file,
            float confidence, int best_matched) :
    ThreadPool(naive_bayes.parallel()),
    _naive_bayes(naive_bayes),
    _correct(0),
    _low_confidence(0) {
    unsigned int total = 0;
    string unknown;
    ifstream ifs(test_file);

    while (getline(ifs, unknown)) {
        total++;

        while (queueSize() >= TEST_MAX_QUEUE_SIZE) {
            sleep(0.5);
        }
        
#ifdef SINGLE_THREADED
        classify_helper(*this, unknown, confidence, best_matched);
#else        
        addTask(new ClassifyTask(*this, unknown, confidence, best_matched));
#endif        
    }

    // Signal the thread pool we are done with populating the jobs.
    done();

    // Wait for the thread pool to finish all the tasks.
    wait();

    // Give a summary of the results.
    unsigned int new_total = total - _low_confidence;
    LOG(INFO)
        << endl
        << "Low Confidence: " << _low_confidence << " / " << total << " = "
        << setprecision(4) << (float)_low_confidence / total * 100 << "%" << endl
        << "Accuracy: " << _correct << " / " << new_total << " = "
        << setprecision(4) << (float)_correct / new_total * 100 << "%"
        << endl;
}
