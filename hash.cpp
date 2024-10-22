#include<bits/stdc++.h>
using namespace std;

#implement the class of Hash Function Here.
#also update the logic used in the readME file.

class HashMap{
private:
    int count;
    int size;

    int hash(string s){
      int hashVal = 0;
      for(char c : s){
        hashVal = 37*hashVal + c;
      }
      return abs(hashVal) % size;
    }
};

class ChainingHashMap: public HashMap{
private:
    vector<Node*> table;

public:
    ChainingHahMap(int s=997): HashMap(s){
        table.resize(size,nullptr);
    }
    ~ChainingHashMap(){
        clear();
    }
}
