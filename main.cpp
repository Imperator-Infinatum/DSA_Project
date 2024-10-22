#include <bits/stdc++.h>
using namespace std;

struct WordFreq {
    string word;
    double spamFreq;
    double hamFreq;
    
    WordFreq(string w = "", double s = 0.0, double h = 0.0) {
        this->word = w;
        this->spamFreq = s;
        this->hamFreq = h; 
    }


};
