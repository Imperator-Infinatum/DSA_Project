# DSA_Project : Group2

How To Use?
Run the file main.cpp
On running the code, A window will get opened where the user will be asked to enter the email he/she wants to classify. After entering the email, the user will click on the classify button./nA new window will get opened which will have the results of both the methods that we have implemented(Chaining and Open Addressing).

We have Implemented 2 hash functions(Chaining and Open Addressing). The HashMap consists of the Word, its spam_count and its ham_count. For each word in the email, we calculate its spam_score (spam_score = spamFreq/(spamFreq + hamFreq)) and then take the average of spam_scores of all the words in the email. If this value is greater than the threshold, the Email is classified as Spam otherwise Ham.
Also, it is possible that a new word which is not present in the HaspMap is encountered in the emial. In such cases, we assign that word, a spam score equal to the threshold value.


Logic For HashFunction:
hashVal = 37 * hashVal + c. 
37 is choosen because 37 is the next larger number which is greater then 26(number of alphabets) and is prime.
Alternatively, we could have choosen 31 or 41 also, but while searching on internet, we found out that 37 is a better choice. Read the below article.
https://medium.com/@chigwel/the-enigma-of-number-37-why-this-prime-number-is-our-intuitions-favorite-168e8947fb3e

