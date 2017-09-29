#include <iostream>
#include <fstream>
#include <string>
#include <glog/logging.h>
#include "test.h"
#include "naive_bayes.h"
#include "json.hpp"
#include "top_queue.h"

//#define SINGLE_THREADED

using namespace std;
using Json = nlohmann::json;

static void classify_helper (Test &test,
                             const string &unknown,
                             const string &label_field,
                             int best_matched)
{
    TopQueue top_queue(best_matched);
    string true_label = "";
  
    test.naive_bayes().classify(unknown, label_field, true_label, top_queue);
        
    bool matched = top_queue.matched(true_label);
    const Prediction &prediction = top_queue.topMatched();

    stringstream ss;
    ss << left
       << setw(16) << true_label
       << setw(16) << prediction.label()
       << setw(10) << (matched ? "Correct" : "Wrong   ")
       << prediction.score()
       << endl;
    LOG(INFO) << ss.str();
    
    if (matched) {
        test.incCorrect();
    }
}

class ClassifyTask : public Task {
private:
    Test &_test;
    const string _unknown;
    const string _label_field;
    int _best_matched;
    
public:
    ClassifyTask(Test &test,
                 const string &unknown, const string &label_field,
                 int best_matched = 1) :
        _test(test),
        _unknown(unknown),
        _label_field(label_field),
        _best_matched(best_matched) {
    }
    void run(void) {
        //cout << _unknown << endl;
        classify_helper(_test, _unknown, _label_field, _best_matched);
    }
};

Test::Test (NaiveBayes &naive_bayes,
            const string &test_file,
            const string &label_field,
            int parallel, int best_matched) :
    ThreadPool(parallel),
    _naive_bayes(naive_bayes),
    _correct(0) {
    int total = 0;
    string unknown;
    ifstream ifs(test_file);

    while (getline(ifs, unknown)) {
        total++;
        
#ifdef SINGLE_THREADED
        classify_helper(*this, unknown, label_field, best_matched);
#else        
        addTask(new ClassifyTask(*this, unknown, label_field, best_matched));
#endif        
    }

    // Signal the thread pool we are done with populating the jobs.
    done();

    // Wait for the thread pool to finish all the tasks.
    wait();

    // Give a summary of the results.
    LOG(INFO) << endl
              << "Accuracy: " << _correct << " / " << total << " = "
              << setprecision(4) << (float)_correct / total * 100 << "%"
              << endl;
}
