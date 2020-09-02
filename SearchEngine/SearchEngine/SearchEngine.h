#pragma once
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

#define cyan 36
#define blueWithBG 44
#define trieCharSize 36 //a->z, 0->9, *#$, 1st

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

	TrieNode() {
	    stopWord = false;
	    fileRoot = nullptr;
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
	void input(ifstream& in, int file);
	void insert(string &Word, int file); //file == -1 if Word is stopword
    void search(string &Word, int ans[], int &count);

	void delPointers(TrieNode* root);

	//for debug
	void display();
	void displayWords(TrieNode* root, string Word);
};
struct SearchEngine {
    int searchEngineNumOfDataFiles;
    vector<string> dataList;
    Trie *root = nullptr;

	void loadDataList(ifstream &in);

	void input();
	void search(string &Word);
	void writeText(int i, string &Word);
	
	vector<Word> breakDown(string txt);
	void input_stop_words(string path);

	//void delPointers();

	//for debug
	void display();
};