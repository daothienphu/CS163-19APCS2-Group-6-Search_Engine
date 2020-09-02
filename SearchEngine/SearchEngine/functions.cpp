#include "SearchEngine.h"
#include "vector"

#pragma region Time Measurement

clock_t ___start_time = clock();

void start() {
    ___start_time = clock();
}

double close() {
    return double(clock()-___start_time) / CLOCKS_PER_SEC;
}

#pragma endregion

#pragma region Utilities

vector<string> stop_words;
string toString(int i) {
	string str;
	if (!i)
		return "0";
	while (i) {
		str = char(i % 10 + '0') + str;
		i /= 10;
	}
	return str;
}
vector<string> split(string queries) {
	vector<string> arr;
	int old_pos = 0;
	for (int i = 1; i < queries.length() - 1; i++)
		if (queries[i] == ' ' && queries[size_t(i) - 1] != ' ' && queries[size_t(i) + 1] != ' ') {
			while (queries[old_pos] == ' ') old_pos++;
			arr.push_back(queries.substr(old_pos, size_t(i) - old_pos));
			old_pos = i;
		}
	while (queries[old_pos] == ' ') old_pos++;
	arr.push_back(queries.substr(old_pos, queries.length() - old_pos));
	return arr;
}
string toLower(string str) {
	for (int i = 0; i < str.length(); ++i)
		if (str[i] < 97)
			str[i] += 32;
	return str;
}
string getValidWord(string wrd) {
	int i = 0;
	while (i < wrd.length()) {
		if ((wrd[i] >= '0' && wrd[i] <= '9') || (wrd[i] >= 'a' && wrd[i] <= 'z') ||
			(wrd[i] >= 'A' && wrd[i] <= 'Z'))
			i++;
		else wrd.erase(i, 1);
	}
	return wrd;
}
string getValidTxt(string str) {
	str = getValidWord(str);
	str = toLower(str);
	return str;
}
//string getFileName(int i) {
//	string fileName = "Data/dataIn";
//	for (int j = 0; j < floor(log10(searchEngineNumOfDataFiles)) - floor(log10(i + 1)); ++j)
//		fileName += "0";
//	fileName += toString(i + 1) + ".txt";
//	return fileName;
//}

string getPrefix(string txt) {
	string pre;
	int i = 0;
	while (i < txt.length()) {
		if ((txt[i] >= '0' && txt[i] <= '9') || (txt[i] >= 'a' && txt[i] <= 'z') || (txt[i] >= 'A' && txt[i] <= 'Z'))
			break;
		else pre += txt[i];
		i++;
	}
	return pre;
}
bool checkStopWord(string txt) {
	for (int i = 0; i < stop_words.size(); i++) if (stop_words[i].compare(txt) == 0) return true;
	return false;
}
string getSuffix(string txt) {
	string suf;
	int i = txt.length() - 1;
	while (i >= 0) {
		if ((txt[i] >= '0' && txt[i] <= '9') || (txt[i] >= 'a' && txt[i] <= 'z') || (txt[i] >= 'A' && txt[i] <= 'Z'))
			break;
		else suf += txt[i];
		i--;
	}
	return suf;
}
void WriteInColor(int color, string text) {
	/*HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO cbInfo;
	GetConsoleScreenBufferInfo(hConsole, &cbInfo);
	int originalColor = cbInfo.wAttributes;
	SetConsoleTextAttribute(hConsole, color);
	cout << text;
	SetConsoleTextAttribute(hConsole, originalColor);*/
	cout << "\x1B[" << color << "m" << text << "\033[0m";
}
#pragma endregion

#pragma region Trie implementation

void Trie::input(ifstream& in) {
    string str;
	do {
		in >> str; //cout << str << " ";
		if(str.size()) insert(str);
	} while (in.good()); //cout << endl;
}
void Trie::insert(string Word) {
	if (!root)
		root = new TrieNode;
	TrieNode* tmp = root;
	Word = getValidTxt(Word);
	for (int i = 0; i < Word.length(); ++i) {
		int subtrahend = (Word[i] >= 'a') ? 'a' : '0' - 26;
		if (!tmp->p[Word[i] - subtrahend])
			tmp->p[Word[i] - subtrahend] = new TrieNode;
		tmp = tmp->p[Word[i] - subtrahend];
	}
	tmp->end = true;
}
bool Trie::search(string Word) {
	if (!root)
		return 0;
	TrieNode* tmp = root;
	Word = getValidTxt(Word);
	for (int i = 0; i < Word.length(); ++i) {
		int subtrahend = (Word[i] >= 'a') ? 'a' : '0' - 26;
		if (!tmp->p[Word[i] - subtrahend])
			return 0;
		tmp = tmp->p[Word[i] - subtrahend];
	}
	if (tmp->end)
		return 1;
	return 0;
}

void Trie::delPointers(TrieNode* root) {
	if (!root) return;
	for (int i = 0; i < trieCharSize; ++i) {
		if (root->p[i])
			delPointers(root->p[i]);
	}
	delete root;
}

//for debug
void Trie::displayWords(TrieNode* root, string Word) {
	if (!root) return;
	if (root->end) {
		cout << Word << " ";
		return;
	}
	for (int i = 0; i < 36; ++i)
		if (root->p[i]) {
			if (i < 26)
				Word += i + 'a';
			else Word += i - 26 + '0';
			displayWords(root->p[i], Word);
			Word.erase(Word.length() - 1);
		}
}
void Trie::display() {
	displayWords(root, "");
}
#pragma endregion

#pragma region Search Engine Algorithms

void SearchEngine::loadDataList(ifstream &in) {
    string filename;
    while (in >> filename)
        if (filename.size()) dataList.push_back(filename);

    searchEngineNumOfDataFiles = dataList.size();
    data = new Trie* [searchEngineNumOfDataFiles] {0};

    cout << "Data list loaded in " << close() << " second(s).\n";
}

void SearchEngine::input() {
    int progress = 0, p;

	for (int i = 0; i < searchEngineNumOfDataFiles; ++i) {
	    p = trunc((double) 100 * i / searchEngineNumOfDataFiles);
	    if (p / 25 > progress) cout << 25 * (++progress) << "% files loaded in " << close() << " second(s).\n";

		string fileName = "Data/" + dataList[i];
		ifstream dataIn{ fileName };
		if (!data[i])
			data[i] = new Trie;
		data[i]->input(dataIn);
	}

    cout << searchEngineNumOfDataFiles << " data files loaded successfully in " << close() << " second(s).\n\n";
}
void SearchEngine::search(string Word) {
	int count = 0;
	for (int i = 0; i < searchEngineNumOfDataFiles; ++i) {
		if (data[i]->search(Word)) {
			writeText(i, Word);
			count++;
		}
		if (count == 5) break;
	}
	if (!count)
	    cout << "No matches found." << endl << endl;
	else
	    cout << count << " matches found in " << close() << " second(s).\n\n";
}
void SearchEngine::writeText(int i, string Word) {
	string fileName = "Data/" + dataList[i];
	ifstream dataIn{ fileName };
	string msg = "Found match(es) from " + fileName;
	WriteInColor(cyan, msg);
	cout << endl;
	string txt;
	while (!dataIn.eof()) {
		dataIn >> txt;
		string prefix = getPrefix(txt);
		string suffix = getSuffix(txt);
		if (getValidTxt(txt) == getValidTxt(Word)) {
			cout << prefix;
			WriteInColor(blueWithBG, getValidWord(txt));
			cout << suffix;
		}
		else cout << txt;
		cout << " ";
	}
	cout << endl << endl;
}
void executeWord(Word& word) {
	switch (word.word[0]) {
	case '#':
		word.function = 2;
		break;
	case '$':
		word.function = 1;
		break;
	}
}
vector<Word> SearchEngine::breakDown(string txt) {
	vector<Word> w;
	vector<string> s = split(txt);
	for (int i = 0; i < s.size(); i++) {
		if (checkStopWord(s[i])) continue;
		Word word(s[i]);
		executeWord(word);
		w.push_back(word);
		if (false) continue;
		cout << "Word ["<<i<<"] :";
		switch (word.function) {
		case 0:
			cout << " (N) ";
			break;
		case 1:
			cout << " ($) ";
			break;
		case 2:
			cout << " (#) ";
			break;
		}
		cout << word.word<<endl;
	}
	return w;
}
void SearchEngine::input_stop_words(string path) {
	ifstream input;
	input.open(path);
	string tmp;
	if (!input.is_open()) {
		cout << "Cannot read stop words"<<endl;
		return;
	}
	while (!input.eof()) {
		input >> tmp;
		{
			stop_words.push_back(tmp);
			tmp = "";
		}
	}
	cout << "Finish reading stop words with " << stop_words.size() << endl;
}

void SearchEngine::delPointers() {
	for (int i = 0; i < searchEngineNumOfDataFiles; ++i) {
		if (data[i])
			data[i]->delPointers(data[i]->root);
	}
	//delete [] data;
}

//for debug
void SearchEngine::display() {
	for (int i = 0; i < searchEngineNumOfDataFiles; ++i) {
		data[i]->display();
		cout << endl << endl;
	}
}
#pragma endregion