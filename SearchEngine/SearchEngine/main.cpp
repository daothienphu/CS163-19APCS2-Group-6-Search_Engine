#include "SearchEngine.h"
#include "iostream"
#include "string"
#include <conio.h>

int* score = new int[11268]{ 0 };

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
	fstream history_f{ BINARY_HISTORY_PATH,ios::out | ios::in | ios::binary };
	history.readTree(history_f, history.root);
	history_f.close();
}
void saveHistory(Trie& history, string queries) {
	if (queries.length() <= 0) return;
	history.insert_sl(queries);

	fstream history_f{ BINARY_HISTORY_PATH,ios::out | ios::in | ios::binary };
	history.saveTree(history_f, history.root);
	history_f.close();
}

int main() {
	ifstream in;
    SearchEngine se;
    start();
    in.open(WORKPLACE + "___index.txt");
	se.loadDataList(in);
	in.close();

	//for (int i = 0; i<se.searchEngineNumOfDataFiles; i++) cout << se.dataList[i] << endl;

    se.input_stop_words("../SearchEngine/Data/stopWords.txt");
	se.input();

	//cout << TrieNode::numTrieNode << endl;
	
	string key = "";
	
	Trie history;
	loadHistory(history);

	vector<string> ui_content;
	ui_content.push_back("                             _        ___                               ");
	ui_content.push_back("    ()                      | |      / (_)             o                ");
	ui_content.push_back("    /\\  _   __,   ,_    __  | |      \\__   _  _    __,     _  _    _    ");
	ui_content.push_back("   /  \\|/  /  |  /  |  /    |/ \\     /    / |/ |  /  | |  / |/ |  |/    ");
	ui_content.push_back("  /(__/|__/\\_/|_/   |_/\\___/|   |_/  \\___/  |  |_/\\_/|/|_/  |  |_/|__/  ");
	ui_content.push_back("                                                    /|                  ");
	ui_content.push_back("                                                    \\|                  ");
	ui_content.push_back("    Insert query here:");
	UI ui;
	ui.content = ui_content;
	//ui.print();
	while (1) {
		char c;
		bool accept = false;
		do {
			//Read input
			c = _getch();
			if ((int)c == 8) {
				if (key.length() > 0) key = key.erase(key.length() - 1, 1);
			}
			else if ((int)c == 80) ui.k++;
			else if ((int)c == 72) ui.k--;
			else if ((int)c == 77) accept = true;
			else if((key.length() < MAX_QUERY_LENGTH) && ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
				(c >= 'A' && c <= 'Z') || c == ' ' || c == '+' || c == '-' || c == '$' || c == '.' || c == ',' || c == '*' || c == '"' || c == ':')) key += c;
			if (ui.k < 0) ui.k = 0;
			//Search
			TrieNode* sug = history.searchSuggestion(key);
			while (!ui.sub_box.empty()) ui.sub_box.pop_back();
			history.getResult(sug, ui.sub_box);
			if (ui.k >= ui.sub_box.size()) ui.k = 0;
			
			if (accept && ui.k < ui.sub_box.size()) {
				//Paste history into search key
				key = ui.sub_box[ui.k];
				accept = false;
			}
			ui.content.back() = "    Search: " + key;
			ui.print();
		} while ((int)c != 13 || key.length() <= 0);
		if (key == "exit")
			break;
		saveHistory(history, key);
		se.search(key, score);
		//Reset and start new search
		cout << "Press any key to start new search";
		key = "";
		accept = false;
		ui.k = 0;
	}
	return 0;
}