#pragma once
#include <iostream>
#include <fstream>
//#include <windows.h>
#include <vector>

using namespace std;

#define cyan 36
#define blueWithBG 44
#define trieCharSize 36 //a->z, 0->9, *#$, 1st
#define searchEngineNumOfDataFiles 10

struct TrieNode {
	TrieNode* p[trieCharSize]{ 0 };
	bool end = false;
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
	void input(ifstream& in);
	void insert(string Word);
	bool search(string Word);

	void delPointers(TrieNode* root);

	TrieNode* getSuggestion(TrieNode* root, string Word);
	TrieNode* searchSuggestion(string Word); 
	void getResult(TrieNode* root, vector<string>& resultSet);
	//for debug
	void display();
	void displayWords(TrieNode* root, string Word);
};
struct SearchEngine {
	Trie* data[searchEngineNumOfDataFiles]{ 0 };

	void input(ifstream& in);
	void search(string Word);
	void writeText(int i, string Word);
	
	vector<Word> breakDown(string txt);
	void input_stop_words(string path);

	void delPointers();

	//for debug
	void display();
};