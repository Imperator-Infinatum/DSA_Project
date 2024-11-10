#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <gtk/gtk.h>
#include <algorithm>

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
    int size;        //total number of buckets in table
    int count;      //count measures the number of buckets used.


    int hash(string key) {
        int hashVal = 0;
        for (char c : key) {
            hashVal = 37 * hashVal + c;    //Why 37? Answered in ReadME file
        }
        return abs(hashVal) % size;
    }

public:
    HashMap(int s = 997) : size(s), count(0) {}      //Initially, since we do not have the maximum count of target words(words except is, this, the, in, or ....), we choose a random number
    virtual ~HashMap() {}

    virtual void insert(WordFreq data) = 0;
    virtual WordFreq* search(string key) = 0;
    virtual void clear() = 0;

    double getLoadFactor() { return (double)count / size; }
    int getCount() { return count; }
};


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

        if (!table[index]) {
            table[index] = newNode;
            count++;
            return;
        }

        Node* current = table[index];
        while (current->next) {
            if (current->data.word == data.word) {
                current->data = data;
                delete newNode;
                return;
            }
            current = current->next;
        }

        if (current->data.word == data.word) {
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

        while (current) {
            if (current->data.word == key) {
                return &(current->data);
            }
            current = current->next;
        }
        return nullptr;
    }

    void clear() override {
        for (Node* head : table) {
            while (head) {
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

        while (i < size) {
            int currentIndex = (index + i) % size;

            if (!table[currentIndex].first) {
                table[currentIndex] = {true, data};
                count++;
                return;
            }
            else if (table[currentIndex].second.word == data.word) {
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

        while (i < size) {
            int currentIndex = (index + i) % size;

            if (!table[currentIndex].first) {
                return nullptr;
            }
            if (table[currentIndex].second.word == key) {
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

        for (const string& word : emailWords) {
            WordFreq* wf = wordMap->search(word);
            if (wf) {
                double totalFreq = wf->spamFreq + wf->hamFreq;
                if (totalFreq > 0) {
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
        if (!token.empty() && token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.length() - 2);
        }
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

    string wordsLine, spamLine, hamLine;
    getline(file, wordsLine);
    getline(file, spamLine);
    getline(file, hamLine);

    vector<string> words = splitCSVLine(wordsLine);
    vector<string> spamCounts = splitCSVLine(spamLine);
    vector<string> hamCounts = splitCSVLine(hamLine);

   
    size_t expectedSize = words.size();

    
    if (spamCounts.size() != expectedSize || hamCounts.size() != expectedSize) {
        cerr << "Error: Inconsistent number of columns in CSV file" << endl;
        return;
    }

    for (size_t i = 0; i < words.size(); ++i) {
        if (words[i].empty() || words[i] == "Word" || words[i] == "word") {
            continue;
        }

        try {
            double spamFreq = stod(spamCounts[i]);
            double hamFreq = stod(hamCounts[i]);

            WordFreq wordFreq(words[i], spamFreq, hamFreq);
            wordMap->insert(wordFreq);
        } catch (const exception& e) {
            cerr << "Error processing column " << i + 1 << ": " << words[i] << endl;
            continue;
        }
    }

    file.close();
}


void saveWordsToCSV(const vector<string>& emailWords, const string& filename) {
    fstream file(filename, ios::in | ios::out | ios::app); 

    if (!file.is_open()) {
        cerr << "Error opening file for saving words: " << filename << endl;
        return;
    }

   
    file.seekp(0, ios::end);
    if (file.tellp() > 0) {
        file.seekp(-1, ios::cur);  
        char lastChar;
        file.get(lastChar);

       
        if (lastChar != '\n') {
            file << ",";
        }
    }

    
    for (size_t i = 0; i < emailWords.size(); ++i) {
        file << "\"" << emailWords[i] << "\""; 

        
        if (i != emailWords.size() - 1) {
            file << ",";
        }
    }

    file << endl; 
    file.close();
}



void show_result_window(const char* chainingResult, const char* openResult) {
    
    GtkWidget* resultWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(resultWindow), "Classification Result");

   
    gtk_window_set_default_size(GTK_WINDOW(resultWindow), 500, 300); 

    
    GtkWidget* resultBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget* chainingLabel = gtk_label_new(chainingResult);
    GtkWidget* openLabel = gtk_label_new(openResult);


    gtk_box_pack_start(GTK_BOX(resultBox), chainingLabel, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(resultBox), openLabel, FALSE, FALSE, 0);

  
    gtk_container_add(GTK_CONTAINER(resultWindow), resultBox);

    
    gtk_widget_show_all(resultWindow);
}


void on_classify_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget* emailTextView = GTK_WIDGET(user_data);  
    
    GtkTextBuffer* textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(emailTextView));

   
    GtkTextIter startIter, endIter;

  
    gtk_text_buffer_get_start_iter(textBuffer, &startIter);

    gtk_text_buffer_get_end_iter(textBuffer, &endIter);

    
    gchar* emailText = gtk_text_buffer_get_text(textBuffer, &startIter, &endIter, FALSE);

    
    ChainingHashMap chainMap(2000);
    OpenAddressingHashMap openMap(2000);

   
    loadWordFrequenciesFromTransposedCSV("C:\\Users\\Komal yadav\\major_dsa\\test.csv", &chainMap);
    loadWordFrequenciesFromTransposedCSV("C:\\Users\\Komal yadav\\major_dsa\\test.csv", &openMap);

    EmailClassifier chainClassifier(&chainMap, 0);
    EmailClassifier openClassifier(&openMap, 0);

    vector<string> emailWords;

   
    stringstream ss(emailText);
    string word;
    while (ss >> word) {
        emailWords.push_back(word);
    }

   
    saveWordsToCSV(emailWords, "C:\\Users\\Komal yadav\\major_dsa\\test.csv");

   
    bool isSpamChain = chainClassifier.classify(emailWords);
    bool isSpamOpen = openClassifier.classify(emailWords);

   
    const char* chainingResult = isSpamChain ? "Chaining: Spam" : "Chaining: Not Spam";
    const char* openResult = isSpamOpen ? "Open Addressing: Spam" : "Open Addressing: Not Spam";

    // Show result window
    show_result_window(chainingResult, openResult);

    g_free(emailText);  
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv); 

    
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Email Classification");


    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600); 

    GtkWidget* classifyButton = gtk_button_new_with_label("Classify");
    GtkWidget* emailTextView = gtk_text_view_new();

    
    gtk_widget_set_size_request(emailTextView, 600, 400); 

    g_signal_connect(classifyButton, "clicked", G_CALLBACK(on_classify_button_clicked), emailTextView);

   
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), emailTextView, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), classifyButton, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
