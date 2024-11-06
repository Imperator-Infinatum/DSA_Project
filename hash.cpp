#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;

struct WordFreq {
    string word;
    double spamFreq;
    double hamFreq;
    
    WordFreq(string w = "", double s = 0.0, double h = 0.0) 
        : word(w), spamFreq(s), hamFreq(h) {}
};

struct Node {
    WordFreq data;
    Node* next;
    
    Node(WordFreq d) : data(d), next(nullptr) {}
};

class HashMap {
protected:
    int size;
    int count;
    
    int hash(string key) {
        int hashVal = 0;
        for(char c : key) {
            hashVal = 37 * hashVal + c;
        }
        return abs(hashVal) % size;
    }
    
public:
    HashMap(int s = 997) : size(s), count(0) {}
    virtual ~HashMap() {}
    
    virtual void insert(WordFreq data) = 0;
    virtual WordFreq* search(string key) = 0;
    virtual void clear() = 0;
    
    double getLoadFactor() { return (double)count / size; }
    int getCount() { return count; }
};

// Chaining implementation
class ChainingHashMap : public HashMap {
private:
    vector<Node*> table;
    
public:
    ChainingHashMap(int s = 997) : HashMap(s) {
        table.resize(size, nullptr);
    }
    
    ~ChainingHashMap() {
        clear();
    }
    
    void insert(WordFreq data) override {
        int index = hash(data.word);
        Node* newNode = new Node(data);
        
        if(!table[index]) {
            table[index] = newNode;
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
        
        if(current->data.word == data.word) {
            current->data = data;
            delete newNode;
            return;
        }
        
        current->next = newNode;
        count++;
    }
    
    WordFreq* search(string key) override {
        int index = hash(key);
        Node* current = table[index];
        
        while(current) {
            if(current->data.word == key) {
                return &(current->data);
            }
            current = current->next;
        }
        return nullptr;
    }
    
    void clear() override {
        for(Node* head : table) {
            while(head) {
                Node* temp = head;
                head = head->next;
                delete temp;
            }
        }
        table.clear();
        count = 0;
    }
};

class OpenAddressingHashMap : public HashMap {
private:
    vector<pair<bool, WordFreq>> table;  
    
public:
    OpenAddressingHashMap(int s = 997) : HashMap(s) {
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
        // Table is full
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

// Email Classifier class
class EmailClassifier {
private:
    HashMap* wordMap;
    double threshold;
    
public:
    EmailClassifier(HashMap* map, double thresh = 0.7) 
        : wordMap(map), threshold(thresh) {}

    
    bool classify(const vector<string>& emailWords) {
        double spamScore = 0.0;
        double totalWords = 0.0;
        
        for(const string& word : emailWords) {
            WordFreq* wf = wordMap->search(word);
            if(wf) {
                double totalFreq = wf->spamFreq + wf->hamFreq;
                if(totalFreq > 0) {
                    spamScore += (wf->spamFreq / totalFreq);
                    totalWords += 1.0;
                }
            }
        }
        
        return (totalWords > 0 && (spamScore / totalWords) >= threshold);
    }
};

vector<string> splitCSVLine(const string& line) {
    vector<string> tokens;
    stringstream ss(line);
    string token;
    
    while (getline(ss, token, ',')) {
        // Remove any quotation marks if present
        if (!token.empty() && token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.length() - 2);
        }
        // Remove any whitespace
        //token.erase(remove_if(token.begin(), token.end(), ::isspace), token.end());
        tokens.push_back(token);
    }
    return tokens;
}

void loadWordFrequenciesFromTransposedCSV(const string& filename, HashMap* wordMap) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    // Read all three rows
    string wordsLine, spamLine, hamLine;
    getline(file, wordsLine);
    getline(file, spamLine);
    getline(file, hamLine);

    // Split each line into tokens
    vector<string> words = splitCSVLine(wordsLine);
    vector<string> spamCounts = splitCSVLine(spamLine);
    vector<string> hamCounts = splitCSVLine(hamLine);

    // Verify all rows have the same number of columns
    if (words.size() != spamCounts.size() || words.size() != hamCounts.size()) {
        cerr << "Error: Inconsistent number of columns in CSV file" << endl;
        cerr << "Words: " << words.size() << ", Spam counts: " << spamCounts.size() << ", Ham counts: " << hamCounts.size() << endl;
        return;
    }

    // Process each column (word)
    for (size_t i = 0; i < words.size(); ++i) {
        try {
            // Skip empty words or header columns
            if (words[i].empty() || words[i] == "Word" || words[i] == "word") {
                continue;
            }

            // Convert counts to doubles
            double spamFreq = stod(spamCounts[i]);
            double hamFreq = stod(hamCounts[i]);

            // Create and insert WordFreq object
            WordFreq wordFreq(words[i], spamFreq, hamFreq);
            wordMap->insert(wordFreq);

        } catch (const exception& e) {
            cerr << "Error processing column " << i + 1 << ": " << words[i] << endl;
            cerr << "Error message: " << e.what() << endl;
            continue;
        }
    }

    file.close();
}


int main() {
    
    ChainingHashMap chainMap(2000);
    OpenAddressingHashMap openMap(2000);
    
    // Load word frequencies from CSV
    cout << "Loading word frequencies into Chaining Hash Map..." << endl;
    loadWordFrequenciesFromTransposedCSV("final.csv", &chainMap);
    cout << "Loaded " << chainMap.getCount() << " words into Chaining Hash Map" << endl;
    cout << "Load factor: " << chainMap.getLoadFactor() << endl;
    
    cout << "\nLoading word frequencies into Open Addressing Hash Map..." << endl;
    loadWordFrequenciesFromTransposedCSV("final.csv", &openMap);
    cout << "Loaded " << openMap.getCount() << " words into Open Addressing Hash Map" << endl;
    cout << "Load factor: " << openMap.getLoadFactor() << endl;

    // Create classifiers
    EmailClassifier chainClassifier(&chainMap);
    EmailClassifier openClassifier(&openMap);

    // Test emails (you can modify these or load from another file)
    vector<pair<string, vector<string>>> testEmails = {
        {"spam", {"money", "free", "win", "cash"}},
        {"ham", {"meeting", "lunch", "tomorrow"}},
        {"spam", {"urgent", "money", "bank", "account"}},
        {"ham", {"project", "deadline", "report"}}
    };

    // Test chaining implementation
    cout << "\nTesting Chaining Implementation:" << endl;
    int correctPredictions = 0;
    for(const auto& email : testEmails) {
        bool prediction = chainClassifier.classify(email.second);
        bool actual = (email.first == "spam");
        cout << "Predicted: " << (prediction ? "spam" : "ham") << ", Actual: " << email.first << endl;
        if(prediction == actual) correctPredictions++;
    }
    cout << "Accuracy: " << (double)correctPredictions/testEmails.size() * 100 << "%" << endl;

    // Test open addressing implementation
    cout << "\nTesting Open Addressing Implementation:" << endl;
    correctPredictions = 0;
    for(const auto& email : testEmails) {
        bool prediction = openClassifier.classify(email.second);
        bool actual = (email.first == "spam");
        cout << "Predicted: " << (prediction ? "spam" : "ham") << ", Actual: " << email.first << endl;
        if(prediction == actual) correctPredictions++;
    }
    cout << "Accuracy: " << (double)correctPredictions/testEmails.size() * 100 << "%" << endl;

    return 0;
}