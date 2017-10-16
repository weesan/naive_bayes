#ifndef TEST_H
#define TEST_H

#include <string>
#include "thread/src/thread.h"

using namespace std;

class NaiveBayes;

class Test : public ThreadPool {
private:
    NaiveBayes &_naive_bayes;
    unsigned int _correct;
    unsigned int _low_confidence;
    Mutex _mutex_correct, _mutex_low_confidence;
    
public:
    Test(NaiveBayes &naive_bayes, const string &test_file,
         float confidence, int best_matched = 1);
    NaiveBayes &naive_bayes(void) const {
        return _naive_bayes;
    }
    void inc_correct(void) {
        _mutex_correct.lock();
        _correct++;
        _mutex_correct.unlock();
    }
    void inc_low_confidence(void) {
        _mutex_low_confidence.lock();
        _low_confidence++;
        _mutex_low_confidence.unlock();
    }
};

#endif // TEST_H
