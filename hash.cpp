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