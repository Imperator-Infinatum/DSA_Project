#include "hashmap.h"
#include <fstream>
#include <sstream>
#include <iostream>

typedef pair<string, vector<string>> EmailData;

// Function to read the header (row of words present) and return the list of words
vector<string> readHeader(const string &filename)
{
    vector<string> headerWords;
    ifstream file(filename);
    string line;

    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return headerWords;
    }

    // Reading the header line
    if (getline(file, line))
    {
        stringstream ss(line);
        string cell;

        // Skipping first column as it is "Email No." header
        getline(ss, cell, ',');

        while (getline(ss, cell, ','))
        {
            headerWords.push_back(cell);
        }
    }

    file.close();
    return headerWords;
}

// Function to read the entire dataset and separate into spam and ham
void readEntireDataset(const string &filename, const vector<string> &headerWords,
                       vector<EmailData> &allSpam, vector<EmailData> &allHam)
{
    ifstream file(filename);
    string line;

    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    // Reading the header line
    getline(file, line);

    while (getline(file, line))
    {
        stringstream ss(line);
        string cell;
        vector<string> cells;

        while (getline(ss, cell, ','))
        {
            cells.push_back(cell);
        }

        if (cells.empty())
            continue;

        // The last column is the label
        int label = stoi(cells.back());

        // Extracting words with non-zero counts
        vector<string> words;
        for (size_t i = 1; i < cells.size() - 1; ++i) // Starting from 1 to skip "Email No."
        {  
            int count = stoi(cells[i]);
            if (count > 0)
            {
                string word = headerWords[i - 1]; // Mapping column to word
                for (int j = 0; j < count; ++j)
                {
                    words.push_back(word);
                }
            }
        }

        if (label == 1)
        {
            allSpam.emplace_back(make_pair("spam", words));
        }
        else
        {
            allHam.emplace_back(make_pair("ham", words));
        }
    }

    file.close();
}
