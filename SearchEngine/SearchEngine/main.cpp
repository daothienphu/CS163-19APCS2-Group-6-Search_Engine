#include "SearchEngine.h"
#include "iostream"
#include "string"
#include <conio.h>

void deleteChar(int k) {
	for(int i = 0; i < k; i++)cout << '\b' << (char)32 << '\b';
}
void WriteColor(int color, string text) {
	/*HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cbInfo;
	GetConsoleScreenBufferInfo(hConsole, &cbInfo);
	int originalColor = cbInfo.wAttributes;
	SetConsoleTextAttribute(hConsole, color);
	cout << text;
	SetConsoleTextAttribute(hConsole, originalColor);*/
	cout << "\x1B[" << color << "m" << text << "\033[0m";
}
int main() {
	ifstream in{ "../Search_Engine/dataList.txt" };
	SearchEngine se;
	se.input(in);
	se.input_stop_words("../Search_Engine/stopWords.txt");
	
	string key = "";
	
	Trie history;
	cout << "What do you want to search? ('exit' to exit) " << endl;
	while (1) {
		char c;
		int t = 0;
		bool accept = false;
		do {
			c = _getch();
			system("cls");
			vector<string> set;
			cout << "What do you want to search? ('exit' to exit) " << endl;
			if ((int)c == 8) {
				if (key.length() > 0) key = key.erase(key.length() - 1, 1);
			}
			else if ((int)c == 80) t++;
			else if ((int)c == 72) t--;
			else if ((int)c == 77) accept = true;
			else if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') || c == ' ') key += c;
			if (t < 0) t = 0;
			if (key.length() > 2) {
				//Average length to do search;
				TrieNode* sug = history.searchSuggestion(key);
				history.getResult(sug, set);
				if (t >= set.size()) t = 0;
			}
			if (accept && t < set.size()) {
				key = set[t];
				accept = false;
			}
			cout <<" >> "<< key << endl;
			for (int i = 0; i < set.size(); i++) {
				cout << "  * ";
				WriteColor(t == i? 44 : 36, set[i]);
				cout << endl;
			}
		} while ((int)c != 13);
		if (key == "exit")
			break;
		cout << key << endl;
		vector<Word> word = se.breakDown(key);

		//Reset and start new search
		//key = "";
	}
	return 0;
}