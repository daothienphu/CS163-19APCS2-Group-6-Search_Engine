#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include <string>
#include <cstring>
#include <queue>
#include <utility>
#include <tuple>

using namespace std;

#define cyan 36
#define blueWithBG 44
#define trieCharSize 39 //a->z, 0->9, *#$, 1st

#define STOPWORD -1
#define INTITLE true

const string WORKPLACE = "../SearchEngine/Data/";
//const string WORKPLACE = "/Users/ducanchu/Documents/Assignments/CS163/CS163-19APCS2-Group-6-SearchEngine/SearchEngine/SearchEngine/Data/";

void start();
double close();

struct PosNode {
    int pos;
    PosNode *Next;
};

struct FileNode {
    int file, num = 0;
    PosNode *posRoot = nullptr;
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
	    inTitleRoot = nullptr;
	    numTrieNode++;
	}
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

	Trie() {
        root = new TrieNode;
        for (int i = 0; i<26; i++) map['a' + i] = map['A' + i] = i;
        for (int i = 0; i<10; i++) map['0' + i] = 26 + i;
        map['*'] = 36;
        map['#'] = 37;
        map['$'] = 38;
	}

	void input(string &filename, int file);
    void input(ifstream& in, int file, bool inTitle = false);
	void insert(string &Word, int file, int pos = -1, bool inTitle = false); //file == -1 if Word is stopword
    void search(string &Word, int ans[], int &count, bool inTitle = false);
	FileNode* searchFilesToScore(string& Word);
	void delPointers(TrieNode* root);

	//for debug
	void display();
	void displayWords(TrieNode* root, string Word);
};
struct SearchEngine {
    int searchEngineNumOfDataFiles;
    vector<string> dataList;
    Trie *root = new Trie;

	void loadDataList(ifstream &in);
	void input_stop_words(string path);
	void input();

	vector<Word> breakDown(string txt);

	void search(string &Word, int*& score);

	void addScore(string query, int*& score);
	void operator3(string query, int*& score);
	void operator5(string query, int*& score);
	void operator9(string query, int*& score);

	void rankResult(int ans[], int &count, int*& score);

	void writeText(int i, vector<string>& queries);
};