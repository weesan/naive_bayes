#ifndef TOP_QUEUE_H
#define TOP_QUEUE_H

#include <queue>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class Prediction {
private:
    string _label;
    double _score;

public:
    Prediction(const string &label, double score) :
        _label(label),
        _score(score)
    {
    }
    const string &label(void) const {
        return _label;
    }
    double score(void) const {
        return _score;
    }
    bool operator<(const Prediction &i) const {
        return _score > i._score;
    }
};

class Predictions : public vector<Prediction> {
};

class TopQueue : public priority_queue<Prediction, Predictions> {
private:
    int _n;
    Predictions _predictions;

private:
    void convertQueue2Vector(void) {
        if (_predictions.size()) {
            return;
        }
        
        while (!empty()) {
            _predictions.push_back(top());
            pop();
        }
        // Since we store the priority queue in ascending order,
        // now, we reverse its order.
        reverse(_predictions.begin(), _predictions.end());
    }
    
public:
    TopQueue(int n = 1) : _n(n){
    }
    const Predictions &predictions(void) const {
        return _predictions;
    }
    void push(const Prediction &prediction) {
        priority_queue<Prediction, Predictions>::push(prediction);
        if (size() > _n) {
            pop();
        }
    }
    bool matched(const string &label) {
        convertQueue2Vector();

        for (int i = 0; i < _predictions.size(); i++) {
            if (_predictions[i].label() == label) {
                return true;
            }
        }

        return false;
    }
    const Prediction &topMatched(void) {
        convertQueue2Vector();
        if (_predictions.size()) {
            return _predictions[0];
        } else {
            static Prediction nullPrediction("", 0.0);
            return nullPrediction;
        }
    }
    void dump(void) {
        while (!empty()) {
            cout << left << setw(16)
                 << top().label() << " " << top().score() << endl;
            pop();
        }
    }
};

#endif // TOP_QUEUE_H
