#pragma once
#include <iostream>
#include <fstream>
#include <windows.h>
#include <vector>
using namespace std;

#define cyan 11
#define blueWithBG 23
#define trieCharSize 36 //a->z, 0->9
#define searchEngineNumOfDataFiles 10

struct TrieNode {
	TrieNode* p[trieCharSize]{ 0 };
	bool end = false;
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

	void delPointers();

	//for debug
	void display();
};