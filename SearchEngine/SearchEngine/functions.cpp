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

bool isValidChar(char &ch) {
    return (ch >= '0' && ch <= '9') ||
           (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
           (ch == '#' || ch == '$' || ch == '*');
}

string toLower(string str) {
	for (int i = 0; i < str.length(); ++i)
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] += 'a' - 'A';
	return str;
}
string getValidWord(string wrd) {
	int i = 0;
	while (i < wrd.length()) {
		if (isValidChar(wrd[i]))
			i++;
		else wrd.erase(i, 1);
	}
	return wrd;
}
string getValidText(string str) {
	str = getValidWord(str);
	str = toLower(str);
	return str;
}

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
//bool checkStopWord(string txt) {
//	for (int i = 0; i < stop_words.size(); i++) if (stop_words[i].compare(txt) == 0) return true;
//	return false;
//}
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
	cout << "\x1B[" << color << "m" << text << "\033[0m";
}
#pragma endregion

#pragma region Trie implementation
int TrieNode::numTrieNode = 0;

void Trie::input(string &filename, int file) {
    FILE *fin = fopen(filename.c_str(), "r");
    if (fin == NULL) return;

    char ch;
    int pos = 0;
    bool inTitle = true, inTitle_tmp = true;
    TrieNode *tmp = root;

    do {
        ch = getc(fin);
        if (ch == '.') inTitle_tmp = false;

        if (ch == ' ' || ch == '\n' || ch == EOF) { //new word
            if (tmp != root) { //check not consecutive space
                if (!tmp->stopWord) {
                    if (tmp->fileRoot == nullptr || tmp->fileRoot->file != file)
                        tmp->fileRoot = new FileNode {file, 0, nullptr, tmp->fileRoot};
                    tmp->fileRoot->posRoot = new PosNode {pos, tmp->fileRoot->posRoot};
                    tmp->fileRoot->num++;

                    if (inTitle) {
                        if (tmp->inTitleRoot == nullptr || tmp->inTitleRoot->file != file)
                            tmp->inTitleRoot = new FileNode {file, 0, nullptr, tmp->inTitleRoot};
                        tmp->inTitleRoot->posRoot = new PosNode {pos, tmp->inTitleRoot->posRoot};
                        tmp->inTitleRoot->num++;
                    }
                }

                //preparing for new word
                tmp = root;
                pos++;
                if (!inTitle_tmp) inTitle = false; //word right before '.' not put in the list inTitleRoot
            }
        } else if (!isValidChar(ch))
            continue;
        else { //valid character
            if (!tmp->p[map[ch]]) tmp->p[map[ch]] = new TrieNode;
            tmp = tmp->p[map[ch]];
        }

    } while (ch != EOF);

    fclose(fin);
}

void Trie::input(ifstream& in, int file, bool inTitle) {
    string str;
    int cnt = 0;
    do {
        in >> str; //cout << file << " " << str << endl;
        if(str.size()) insert(str, file, cnt++, inTitle);
    } while (in.good()); //cout << endl;
}

void Trie::insert(string &Word, int file, int pos, bool inTitle) {
	//if (!root) root = new TrieNode;

	TrieNode* tmp = root;
	Word = getValidText(Word); //cout << file << " " << Word << endl;
	for (int i = 0; i < Word.length(); ++i) {
		//int subtrahend = (Word[i] >= 'a') ? 'a' : '0' - 26;
		if (!tmp->p[map[Word[i]]])
			tmp->p[map[Word[i]]] = new TrieNode;
		tmp = tmp->p[map[Word[i]]];
	}

	if (tmp->stopWord) return;
	if (file == -1)
	    tmp->stopWord = true;
	else {
	    if (tmp->fileRoot == nullptr || tmp->fileRoot->file != file)
            tmp->fileRoot = new FileNode {file, 0, nullptr, tmp->fileRoot};
	    tmp->fileRoot->posRoot = new PosNode {pos, tmp->fileRoot->posRoot};
	    tmp->fileRoot->num++;

	    if (inTitle) {
            if (tmp->inTitleRoot == nullptr || tmp->inTitleRoot->file != file)
                tmp->inTitleRoot = new FileNode {file, 0, nullptr, tmp->inTitleRoot};
            tmp->inTitleRoot->posRoot = new PosNode {pos, tmp->inTitleRoot->posRoot};
            tmp->inTitleRoot->num++;
	    }
	}
}
void Trie::search(string &Word, int ans[], int &count, bool inTitle) {
	if (!root) return;
	TrieNode* tmp = root;
	Word = getValidText(Word);
	for (int i = 0; i < Word.length(); ++i) {
		//int subtrahend = (Word[i] >= 'a') ? 'a' : '0' - 26;
		if (!tmp->p[map[Word[i]]]) return;
		tmp = tmp->p[map[Word[i]]];
	}

	if (tmp->stopWord) return;
	FileNode *fileRoot = (inTitle ? tmp->inTitleRoot : tmp->fileRoot);
	for (FileNode *p = fileRoot; p != nullptr && count < 5; p = p->Next) ans[count++] = p->file;
}

//basically search, but returns FileNode =)))
FileNode* Trie::searchFilesToScore(string& Word) {
	if (!root) return nullptr;
	TrieNode* tmp = root;
	Word = getValidText(Word);
	for (int i = 0; i < Word.length(); ++i) {
		int subtrahend = (Word[i] >= 'a') ? 'a' : '0' - 26;
		if (!tmp->p[Word[i] - subtrahend]) return nullptr;
		tmp = tmp->p[Word[i] - subtrahend];
	}

	if (tmp->stopWord) return nullptr;
	return tmp->fileRoot;
}

#pragma endregion

#pragma region Search Engine Algorithms
void SearchEngine::loadDataList(ifstream &in) {
    string filename;
	while (getline(in, filename)) //changed this from in >> filename
		if (filename.size()) dataList.push_back(filename);

    searchEngineNumOfDataFiles = dataList.size();

    cout << "Data list loaded in " << close() << " second(s).\n";
}
void SearchEngine::input_stop_words(string path) {
	ifstream input;
	input.open(path);

	string stopWords;
	int numStopWords = 0;

	if (!input.is_open()) {
		cout << "Cannot read stop words" << endl;
		return;
	}

	while (!input.eof()) {
		input >> stopWords;
        root->insert(stopWords, STOPWORD);
        numStopWords++;
	}

	cout << "Finish reading stop words with " << numStopWords << " stop words in " << close() << " second(s).\n";
}
void SearchEngine::input() {
    int progress = 0, p;
    string fileName;

    for (int i = 0; i < searchEngineNumOfDataFiles; ++i) {
	    p = trunc((double) 100 * i / searchEngineNumOfDataFiles);
	    if (p / 25 > progress) cout << 25 * (++progress) << "% files loaded in " << close() << " second(s).\n";

		fileName = WORKPLACE + dataList[i];
		//ifstream dataIn{ fileName };
		root->input(fileName, i);
		//root->input(dataIn, i);
	}

    cout << searchEngineNumOfDataFiles << " data files loaded successfully in " << close() << " second(s).\n\n";
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
		//if (checkStopWord(s[i])) continue;
		Word word(s[i]);
		executeWord(word);
		w.push_back(word);
		if (false) continue;
		cout << "Word [" << i << "] :";
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
		cout << word.word << endl;
	}
	return w;
}


void SearchEngine::search(string &Word, int*& score) {
	int count = 0, ans[5];
//
//    root->search(Word, ans, count);
//    for (int i = 0; i<count; i++) writeText(ans[i], Word);

	memset(score, 0, searchEngineNumOfDataFiles * sizeof(int));

	vector<string> splittedQuery = split(Word);

	//double time = close(), time1;
	for (int i = 0; i < splittedQuery.size(); i++) {
		if (splittedQuery[i][0] == '-')
			operator3(splittedQuery[i].substr(1 , splittedQuery[i].length() - 1), score);
		else if (splittedQuery[i][0] == '+')
			operator5(splittedQuery[i].substr(1, splittedQuery[i].length() - 1), score);
		else
			addScore(splittedQuery[i], score);

		//time1 = close();
		//cout << time1 - time << endl;
		//time = time1;
	}

	rankResult(ans, count, score);

	vector<string> queryToHighlight; //will change later
	for (int i = 0; i < splittedQuery.size(); ++i)
		if (splittedQuery[i][0] != '-')
			queryToHighlight.push_back(splittedQuery[i]);
	
	for (int i = 0; i < count; ++i)
		writeText(ans[i], queryToHighlight);

	if (!count)
	    cout << "No matches found in " << close() << " second(s).\n\n";
	else
	    cout << count << " matches found in " << close() << " second(s).\n\n";
}
//used for general case
void SearchEngine::addScore(string query, int*& score) {
	FileNode* files;
	ifstream fileIn;
	files = root->searchFilesToScore(query);
	query = getValidText(query);
	for (files; files != nullptr; files = files->Next) {
//		fileIn.open(WORKPLACE + dataList[files->file]);
//		string word = "";
//		if (fileIn.is_open()) {
//			while (!fileIn.eof()) {
//				fileIn >> word;
//				if (getValidText(word) == query && score[files->file] != -1)
//					score[files->file]++;
//			}
//		}
//		fileIn.close();

        score[files->file] += files->num;
	}
};
//only used for the word behind "+" operator
void SearchEngine::operator5(string query, int*& score) {
	FileNode* files;
	files = root->searchFilesToScore(query);
	for (files; files != nullptr; files = files->Next)
		score[files->file] -= 10000;
	for (int i = 0; i < searchEngineNumOfDataFiles; ++i)
		if (score[i] < 5000) score[i] += 10000;
		else score[i] = -1;
}
//only used for the word behind "-" operator
void SearchEngine::operator3(string query, int*& score) {
	FileNode* files;
	files = root->searchFilesToScore(query);
	for (files; files != nullptr; files = files->Next)
		score[files->file] = -1;
}


void SearchEngine::rankResult(int ans[], int &count, int*& score) {
	count = 0;
	int k = 0;
	for (int i = 0; i < 5; ++i) {
		int max = 0;
		for (int j = 1; j < searchEngineNumOfDataFiles; ++j)
			if (score[j] > score[max])
				max = j;
		if (score[max] <= 0) break;
		count++;
		ans[k++] = max;
		score[max] = -1;
	}
}

void SearchEngine::writeText(int i, vector<string>& queries) {
	//string fileName = "../SearchEngine/Data/" + dataList[i];
    string fileName = WORKPLACE + dataList[i];
	ifstream dataIn{ fileName };
	string msg = "Found match(es) from " + fileName;
	WriteInColor(cyan, msg);
	cout << endl;
	string txt;
	while (!dataIn.eof()) {
		dataIn >> txt;
		string prefix = getPrefix(txt);
		string suffix = getSuffix(txt);
		bool isQueryWord = 0;
		for (int i = 0; i < queries.size(); ++i) {
			if (getValidText(txt) == getValidText(queries[i])) {
				cout << prefix;
				WriteInColor(blueWithBG, getValidWord(txt));
				cout << suffix;
				isQueryWord = 1;
				break;
			}
		}
		if (!isQueryWord) cout << txt;
		cout << " ";
	}
	cout << endl << endl;
}
#pragma endregion
