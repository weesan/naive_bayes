#ifndef TRAIN_H
#define TRAIN_H

#include <string>
#include "thread/src/thread.h"

using namespace std;

class NaiveBayes;

class Train : ThreadPool {
private:
    NaiveBayes &_naive_bayes;

public:
    Train(NaiveBayes &naive_bayes, const string &train_file);
    NaiveBayes &naive_bayes(void) const {
        return _naive_bayes;
    }
};

#endif // TRAIN_H
