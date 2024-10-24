#include<bits/stdc++.h>
using namespace std;

#implement the class of Hash Function Here.
#also update the logic used in the readME file.

class HashMap{
protected:
    int count;
    int size;

    int hash(string s){
      int hashVal = 0;
      for(char c : s){
        hashVal = 37*hashVal + c;
      }
      return abs(hashVal) % size;
    }

public:
    HashMap(int sz = 997){
        this->size = sz;
        this->count = 0;
};

struct Node {
    WordFreq data;      //the wordFreq class in defiened in main
    Node* next;
    
    Node(WordFreq d){
        data = d;
        next = nullptr;
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
        while(current->next) {
            if(current->data.word == data.word) {
                current->data = data;
                delete newNode;
                return;
            }
            current = current->next;
        }
        
        
        current->next = newNode;
        count++;
    }
}

class OpenAddressingHashMap : public HashMap {
private:
    vector<pair<bool, WordFreq>> table;  // bool indicates if slot is occupied
    
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
