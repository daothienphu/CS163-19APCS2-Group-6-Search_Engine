#include "SearchEngine.h"
#include "iostream"
#include "string"
#include <conio.h>

int* score = new int[11268]{ 0 };

#define HISTORY_PATH "../SearchEngine/Data/history.txt"
#define STOPWORD_PATH "../SearchEngine/Data/stopWords.txt"
#define DATA_PATH "../SearchEngine/Data/dataList.txt"

void deleteChar(int k) {
	for (int i = 0; i < k; i++)cout << '\b' << (char)32 << '\b';
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
void loadHistory(Trie& history) {
	ifstream input{ HISTORY_PATH };
	string tmp; int k = 0;
	if (!input.is_open()) {
		cout << "Cannot read stop words" << endl;
		return;
	}
	while (!input.eof()) {
		getline(input, tmp);
		if (tmp.length() < 1) break;
		k++;
		history.insert_sl(tmp);
	}
	cout << "[DEBUG] Loaded history " << k << endl;
}
void saveHistory(Trie& history, string queries) {
	history.insert_sl(queries);
	ofstream out;
	out.open(HISTORY_PATH, ios::app);
	out << queries << endl;
}

int main() {
	ifstream in;
    SearchEngine se;

    start();
	in.open("../SearchEngine/Data/___index.txt");
	se.loadDataList(in);
	in.close();

	//for (int i = 0; i<se.searchEngineNumOfDataFiles; i++) cout << se.dataList[i] << endl;

    se.input_stop_words("../SearchEngine/Data/stopWords.txt");
	se.input();

	cout << TrieNode::numTrieNode << endl;
	
	string key = "";
	
	Trie history;
	loadHistory(history);

	cout << "What do you want to search? ('exit' to exit) " << endl;
	while (1) {
		char c;
		int t = 0;
		bool accept = false;
		do {
			//Read input
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
			//Search
			TrieNode* sug = history.searchSuggestion(key);
			history.getResult(sug, set);
			if (t >= set.size()) t = 0;
			
			if (accept && t < set.size()) {
				//Paste history into search key
				key = set[t];
				accept = false;
			}
			
			//Print out the search
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
		saveHistory(history, key);
		vector<Word> word = se.breakDown(key);

		//Reset and start new search
		cout << "Press any key to start new search";
		key = "";
		accept = false;
		t = 0;
	}
	return 0;
}