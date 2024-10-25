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

struct Node {
    WordFreq data;      //the wordFreq class in defiened in main
    Node* next;
    
    Node(WordFreq d){
        data = d;
        next = nullptr;
    }
};

class HashMap{
protected:
    int count;
    int size;

    int hash(string s){
    int hashVal = 0;
    for(char c : s){
        hashVal = 37*hashVal + c;       //Why 37? Answered in ReadME file
    }
      return abs(hashVal) % size;
    }

public:
    HashMap(int sz = 997){        //Initially, since we do not have the maximum count of target words(words except is, this, the, in, or ....), we choose a random number.
        this->size = sz;
        this->count = 0;
    }
};



class ChainingHashMap: public HashMap{
private:
    vector<Node*> table;

public:
    ChainingHashMap(int s=997){
        HashMap(s);
        table.resize(size,nullptr);
    }
    ~ChainingHashMap(){
        clear();
    }

    void insert(WordFreq data) override{
        int index=hash(data.word);
        Node* newNode=new Node(data);

        if(!table[index]){
            table[index]=newNode;
            count++;
            return;
        }
        Node* current = table[index];
        while(current->next != NULL) {
            if(((current->data).word) == data.word) {
                current->data = data;
                delete newNode;
                return;
            }
            current = current->next;
        }
        
        current->next = newNode;
        count++;
    }
};

class OpenAddressingHashMap : public HashMap {
private:
    vector<pair<bool, WordFreq>> table;  
    
public:
    OpenAddressingHashMap(int s = 997){
        HashMap(s);
        table.resize(size, {false, WordFreq()});
    }
    
    void insert(WordFreq data) override {
        int index = hash(data.word);
        int i = 0;
        
        while(i < size) {
            int currentIndex = (index + i) % size;
            
            if(!table[currentIndex].first) {
                table[currentIndex] = {true, data};
                count++;
                return;
            }
            else if(table[currentIndex].second.word == data.word) {
                table[currentIndex].second = data;
                return;
            }
            i++;
        }
        
        cout << "Hash table is full!" << endl;
    }
    
    WordFreq* search(string key) override {
        int index = hash(key);
        int i = 0;
        
        while(i < size) {
            int currentIndex = (index + i) % size;
            
            if(!table[currentIndex].first) {
                return nullptr;
            }
            if(table[currentIndex].second.word == key) {
                return &(table[currentIndex].second);
            }
            i++;
        }
        return nullptr;
    }
    
    void clear() override {
        table.clear();
        table.resize(size, {false, WordFreq()});
        count = 0;
    }
};

class EmailClassifier
{
private:
    HashMap *wordMap;
    double threshold;

public:
    EmailClassifier(HashMap *map, double thresh = 0.7){
        this->wordMap = map;
        this->threshold = thresh;
    }

    void train(vector<pair<string, vector<string>>> &trainingData, bool isSpam)
    {
        for (const auto &email : trainingData)
        {
            for (const string &word : email.second)
            {
                WordFreq *wf = wordMap->search(word);
                if (!wf)
                {
                    WordFreq newWF(word);
                    if (isSpam)
                        newWF.spamFreq = 1.0;
                    else
                        newWF.hamFreq = 1.0;
                    wordMap->insert(newWF);
                }
                else
                {
                    if (isSpam)
                        wf->spamFreq += 1.0;
                    else
                        wf->hamFreq += 1.0;
                }
            }
        }
    }

    bool classify(const vector<string> &emailWords)
    {
        double spamScore = 0.0;
        double totalWords = 0.0;

        for (const string &word : emailWords)
        {
            WordFreq *wf = wordMap->search(word);
            if (wf)
            {
                double totalFreq = wf->spamFreq + wf->hamFreq;
                if (totalFreq > 0)
                {
                    spamScore += (wf->spamFreq / totalFreq);
                    totalWords += 1.0;
                }
            }
        return (totalWords > 0 && (spamScore / totalWords) >= threshold);
    }
};
