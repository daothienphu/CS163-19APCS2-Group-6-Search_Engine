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

struct FileNode {
    int file, num = 0;
	vector<int> pos;
    FileNode *Next;
	FileNode(int f, int n, FileNode* ne) {
		file = f;
		num = n;
		Next = ne;
	}
};
struct SearchTask {
	int function = -1;
	vector<string> words;
	vector<string> words2;
	bool isEmpty() {
		return (words.size() == 0 && words2.size() == 0);
	}
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
	string s;
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

	void insert_sl(string Word);


	void input(string &filename, int file);
    void input(ifstream& in, int file, bool inTitle = false);
	void insert(string &Word, int file, int pos = -1, bool inTitle = false); //file == -1 if Word is stopword
    void search(string &Word, int ans[], int &count, bool inTitle = false);
	
	bool isStopWord(string& Word);

	FileNode* searchFilesToScore(string& Word);

	TrieNode* getSuggestion(TrieNode* root, string Word);
	TrieNode* searchSuggestion(string Word); 
	void getResult(TrieNode* root, vector<string>& resultSet);
};
struct SearchEngine {
    int searchEngineNumOfDataFiles;
    vector<string> dataList;
    Trie *root = new Trie;

	void loadDataList(ifstream &in);
	void input_stop_words(string path);
	void input();

	vector<SearchTask> breakDown(string txt);

	void search(string &Word, int*& score);

	void addScore(string query, int*& score);
	void operator3(string query, int*& score);
	void operator5(string query, int*& score);
	void operator9(vector<string> query, int*& score);

	void rankResult(int ans[], int &count, int*& score);

	void writeText(int i, vector<string>& queries);
};