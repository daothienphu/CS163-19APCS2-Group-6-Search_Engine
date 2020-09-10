#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>

using namespace std;

#define cyan 36
#define blueWithBG 44
#define trieCharSize 39 //a->z, 0->9, *#$, 1st

#define STOPWORD -1
#define INTITLE true

void start();
double close();

struct FileNode {
    int file;
    FileNode *Next;
};

struct TrieNode {
    static int numTrieNode;
	TrieNode* p[trieCharSize]{ 0 };

	bool stopWord = false;
	FileNode *fileRoot = nullptr;
	FileNode *inTitleRoot = nullptr;

	TrieNode() {
	    stopWord = false;
	    fileRoot = nullptr;
	    numTrieNode++;
	}
	string s;
};

struct Word {
	string word;
	int v1 = -1, v2 = -1;
	int function = 0;

	Word(string w) {
		word = w;
	}
	/*
	Function
	1 - Normal word
	2 - $ Price tag
	3 - # Hash tag
	4 - * Place holder
	5 - $..$ Range price
	*/
};

struct Trie {
	TrieNode* root = nullptr;

	int map[255];

	void insert_sl(string Word);



	Trie();

	void input(ifstream& in, int file, bool inTitle = false);
	void insert(string &Word, int file, bool inTitle = false); //file == -1 if Word is stopword
    void search(string &Word, int ans[], int &count, bool inTitle = false);
	FileNode* searchFilesToScore(string& Word);
	void delPointers(TrieNode* root);

	TrieNode* getSuggestion(TrieNode* root, string Word);
	TrieNode* searchSuggestion(string Word); 
	void getResult(TrieNode* root, vector<string>& resultSet);
	//for debug
	void display();
	void displayWords(TrieNode* root, string Word);
};
struct SearchEngine {
    int searchEngineNumOfDataFiles;
    vector<string> dataList;
    Trie *root = nullptr;

	void loadDataList(ifstream &in);
	void input_stop_words(string path);
	void input();

	vector<Word> breakDown(string txt);

	void search(string &Word, int*& score);

	void addScore(string query, int*& score);
	void operator3(string query, int*& score);
	void operator5(string query, int*& score);

	void rankResult(int ans[], int &count, int*& score);

	void writeText(int i, vector<string>& queries);
};