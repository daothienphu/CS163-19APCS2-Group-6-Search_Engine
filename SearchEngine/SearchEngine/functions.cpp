#include "SearchEngine.h"

using namespace std;

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
			arr.emplace_back(queries.substr(old_pos, size_t(i) - old_pos));
			old_pos = i;
		}
	while (queries[old_pos] == ' ') old_pos++;
	arr.emplace_back(queries.substr(old_pos, queries.length() - old_pos));
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

bool Trie::isStopWord(string Word) {
	TrieNode* tmp = root;
	Word = getValidText(Word);
	for (int i = 0; i < Word.length(); i++) {
		if (tmp->p[map[Word[i]]])
			tmp = tmp->p[map[Word[i]]];
		else return false;
	}
	return tmp->stopWord;
}

void Trie::input(string &filename, int file) {
	#pragma warning(suppress : 4996)
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
                        tmp->fileRoot = new FileNode (file, tmp->fileRoot);
					tmp->fileRoot->pos.emplace_back(pos);
                    if (inTitle) {
                        if (tmp->inTitleRoot == nullptr || tmp->inTitleRoot->file != file)
                            tmp->inTitleRoot = new FileNode (file, tmp->inTitleRoot);
						tmp->inTitleRoot->pos.emplace_back(pos);
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
            tmp->fileRoot = new FileNode (file, tmp->fileRoot);
		tmp->fileRoot->pos.emplace_back(pos);

	    if (inTitle) {
            if (tmp->inTitleRoot == nullptr || tmp->inTitleRoot->file != file)
                tmp->inTitleRoot = new FileNode (file, tmp->inTitleRoot);
			tmp->inTitleRoot->pos.emplace_back(pos);
	    }
	}
}
void Trie::search(string &Word, int ans[], int &count, bool inTitle) {
	if (!root) return;
	TrieNode* tmp = root;
	string tempWord = Word;
	Word = getValidText(Word);
	for (int i = 0; i < Word.length(); ++i) {
		if (!tmp->p[map[Word[i]]]) return;
		tmp = tmp->p[map[Word[i]]];
	}

	if (tmp->stopWord) return;
	FileNode *fileRoot = (inTitle ? tmp->inTitleRoot : tmp->fileRoot);
	for (FileNode *p = fileRoot; p != nullptr && count < 5; p = p->Next) ans[count++] = p->file;
}
void Trie::insert_sl(string Word) {
	if (!root)
		root = new TrieNode;
	TrieNode* tmp = root;
	string tempWord = Word;
	Word = getValidText(Word);
	if (Word.length() <= 0) return;
	for (int i = 0; i < Word.length(); ++i) {
		if (!tmp->p[map[Word[i]]])
			tmp->p[map[Word[i]]] = new TrieNode;
		tmp = tmp->p[map[Word[i]]];
	}
	tmp->s = tempWord;
}
//basically search, but returns FileNode =)))
FileNode* Trie::searchFilesToScore(string& Word, bool intitle) {
	if (!root) return nullptr;
	TrieNode* tmp = root;
	Word = getValidText(Word);
	for (int i = 0; i < Word.length(); ++i) {
		if (!tmp->p[map[Word[i]]]) return nullptr;
		tmp = tmp->p[map[Word[i]]];
	}
	if (tmp->stopWord) return nullptr;
	return (intitle ? tmp->inTitleRoot : tmp->fileRoot);
}
string getCorrectString(string& s,int i) {
	while (s.length() < i) s += ' ';
	return s;
}
void Trie::saveTree(fstream& out, TrieNode* root) {
	if (!out.is_open() ||root == nullptr) return;
	int size = root->s.size();
	out.write((char*)&size, sizeof(size));
	out.write(root->s.c_str(), root->s.size());
	string content = "";
	for (int i = 0; i < trieCharSize; i++) content += (root->p[i] != nullptr ? '1' : '0');
	out.write(content.c_str(), trieCharSize);
	for (int i = 0; i < trieCharSize; i++) saveTree(out, root->p[i]);
}
void Trie::readTree(fstream& in, TrieNode* root) {
	if (!in.is_open()) return;
	string content;
	int size = 0;
	in.read((char*)&size, sizeof(size));
	content.resize(size);
	in.read((char*)&content[0], size);
	root->s = content.c_str();
	content.resize(trieCharSize);
	in.read((char*)&content[0], trieCharSize);
	for (int i = 0; i < trieCharSize; i++) {
		if (content[i] == '1') {
			root->p[i] = new TrieNode;
			readTree(in, root->p[i]);
		}
	}
}
TrieNode* Trie::getSuggestion(TrieNode* root, string Word) {
	if (root == nullptr) return root;
	if (Word.length() == 0) return root;
	if (root->p[map[Word[0]]]) {
		char tmp = Word[0];
		Word = Word.erase(0,1);
		return Trie::getSuggestion(root->p[map[tmp]], Word);
	}
	return nullptr;
}
#define MAX_RESULT 6
void Trie::getResult(TrieNode* root, vector<string>& resultSet) {
	if (!root) return;
	if (root->s.length() > 0) resultSet.push_back(root->s);
	if (resultSet.size() >= MAX_RESULT) return;
	for (int i = 0; i < trieCharSize; ++i)
		if (root->p[i]) 
			getResult(root->p[i], resultSet);
}
TrieNode* Trie::searchSuggestion(string Word) {
	return getSuggestion(root, getValidText(Word));
}
#pragma endregion

#pragma region Search Engine Algorithms
void SearchEngine::loadDataList(ifstream &in) {
    string filename;
	while (getline(in, filename)) //changed this from in >> filename
		if (filename.size()) dataList.emplace_back(filename);

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

int getFlag(string Word) {
	if (Word == "AND") return 0;
	if (Word == "OR") return 1;
	if (Word.length() >= 7 && Word.substr(0, 7) == "intitle") return 6;
	if (Word.length() >= 8 && Word.substr(0, 8) == "filetype") return 7;
	switch (Word[0]) {
	case '$':
		return 2;
	case '+':
		return 3;
	case '-':
		return 4;
	case '"':
		return 5;
	}
	switch (Word[Word.length() - 1]) {
	case '$':
		return 2;
	case '+':
		return 3;
	case '-':
		return 4;
	case '"':
		return 5;
	}
	return -1;
}
void removeParamenter(string& str, char c) {
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
}
vector<SearchTask> SearchEngine::breakDown(string txt) {
	vector<SearchTask> w;
	w.push_back(SearchTask());
	vector<string> s = split(txt);
	bool need_push = false;
	for (int i = 0; i < s.size(); i++) {
		if (root->isStopWord(s[i]) && w.back().function == 5) s[i] = "*"; //Check for stop word for function 5
		if (need_push) {
			w.push_back(SearchTask());
			need_push = false;
		}
		int flag = getFlag(s[i]);
		if (flag == -1) {
			if (w.back().function < 2 && w.back().function > -1) w.back().words2.push_back(s[i]);
			else w.back().words.push_back(s[i]);
		}
		else {
			if (flag == 7) {
				if (!w.back().isEmpty()) w.push_back(SearchTask());
				w.back().function = flag;
				w.back().words.push_back(s[i].substr(9, s[i].length() - 9)); //filetype:
				need_push = true;
				continue;
			}
			if (flag == 6) {
				if (!w.back().isEmpty() && w.back().function != 6) w.push_back(SearchTask());
				w.back().function = flag;
				w.back().words.push_back(s[i].substr(8, s[i].length() - 8)); //intitle:
				continue;
			}
			if(flag > 2 && flag < 5) {
				if (!w.back().isEmpty()) w.push_back(SearchTask());
				w.back().function = flag;
				removeParamenter(s[i], '+');
				removeParamenter(s[i], '-');
				w.back().words.push_back(getValidText(s[i]));
				need_push = true;
				continue;
			}
			if (flag == 5) {
				removeParamenter(s[i], '"');
				w.back().words.push_back(getValidText(s[i]));
				if (w.back().function == flag) {
					w.push_back(SearchTask());
					continue;
				}
				w.back().function = flag;
				continue;
			}
			if (flag == 2) {
				w.back().function = flag;
				removeParamenter(s[i], '$');
				w.back().words.push_back(getValidText(s[i]));
				need_push = true;
				continue;
			}
			w.back().function = flag;
		}
	}
	return w;
}

void SearchEngine::search(string &Word, int*& score) {
	int count = 0, ans[5];

	vector<SearchTask> tasks = breakDown(Word);
	vector<string> queryToHighlight;
	memset(score, 0, searchEngineNumOfDataFiles * sizeof(int));
	for (int i = 0; i < tasks.size(); i++) {
		if (true) {
			cout << "Task: " << tasks[i].function << " ";
			for (int j = 0; j < tasks[i].words.size(); j++) cout << tasks[i].words[j] << " ";
			cout << "|";
			for (int j = 0; j < tasks[i].words2.size(); j++) cout << tasks[i].words2[j] << " ";
			cout << endl;
		}
		//double time = close(), time1;
		switch (tasks[i].function) {
		case 5:
			operator9(tasks[i].words, score);//operator10 * also search exact
			break;
		case 1: //OR
			operator9(tasks[i].words, score);
			operator9(tasks[i].words2, score);
			break;
		case 0: //AND

			break;
		}
		for (int k = 0; k < tasks[i].words.size(); k++) {
			switch (tasks[i].function) {
			case 4:
				operator3(tasks[i].words[k], score);//  -
				break;
			case 3:
				operator5(tasks[i].words[k], score);//  +
				break;
			case 6:
				operator4(tasks[i].words[k], score);//  intitle:
				break;
			case 7:
				operator6(tasks[i].words[k], score);//  filetype:
				break;
			case -1:
				addScore(tasks[i].words[k], score);
			}
			if (tasks[i].function != 4 && tasks[i].function != 7)
				queryToHighlight.emplace_back(tasks[i].words[k]);
			//time1 = close();
			//cout << time1 - time << endl;
			//time = time1;
		}
	}
	rankResult(ans, count, score);

	for (int i = 0; i < count; ++i) {
		cout << score[ans[i]]*-1 << endl;
		writeText(ans[i], queryToHighlight);
	}
	if (!count)
	    cout << "No matches found in " << close() << " second(s).\n\n";
	else
	    cout << count << " matches found in " << close() << " second(s).\n\n";
}
//used for general case
void SearchEngine::addScore(string query, int*& score) {
	FileNode* files;
	files = root->searchFilesToScore(query);
	query = getValidText(query);
	for (files; files != nullptr; files = files->Next) {
        if(score[files->file] >= 0) score[files->file] += files->pos.size();
	}
};
//only used for the word behind "filetype:" operator
void SearchEngine::operator6(string filetype, int*& score) {
	for (int i = 0; i < searchEngineNumOfDataFiles; i++)
		if (dataList[i].substr(dataList[i].length() - filetype.length(), filetype.length()) != filetype)
			score[i] = -1;
}
//only used for the word behind "+" operator
void SearchEngine::operator5(string query, int*& score) {
	FileNode* files;
	files = root->searchFilesToScore(query);
	
	for (int i = searchEngineNumOfDataFiles - 1; i >= 0 && files != nullptr; i--)
		if (i == files->file) {
			score[i] += files->pos.size();
			files = files->Next;
		}
		else
			score[i] = -1;
}
//only used for the word behind "intitle:" operator
void SearchEngine::operator4(string query, int*& score) {
	FileNode* files;
	files = root->searchFilesToScore(query, true); //search inTitle

	for (files; files != nullptr; files = files->Next)
		score[files->file]++;
}
//only used for the word behind "-" operator
void SearchEngine::operator3(string query, int*& score) {
	FileNode* files;
	files = root->searchFilesToScore(query);

	for (files; files != nullptr; files = files->Next)
		score[files->file] = -1;
}
FileNode* getFileNode(FileNode* root, int index) {
	if (root == nullptr) return nullptr;
	if (root->file == index) return root;
	return getFileNode(root->Next, index);
}
void SearchEngine::operator9(vector<string> query, int*& score) {
	//For triming the *
	while (query.size() > 0 && query.back() == "*") query.pop_back();
	if (query.size() <= 0) return;
	FileNode* old = root->searchFilesToScore(query[0]);
	if (old == nullptr)	return;
	for (int i = 1; i < query.size(); i++) {
		if (query[i] == "*") continue;
		FileNode* files;
		files = root->searchFilesToScore(query[i]);
		bool flag = false;
		for (files; files != nullptr; files = files->Next) {
			FileNode* in_old = getFileNode(old, files->file);
			if (in_old == nullptr) continue;
			for (int k = 0; k < in_old->pos.size(); k++) for (int j = 0; j < files->pos.size(); j++) {
				if (files->pos[j] - in_old->pos[k] == i) {
					flag = true;
					if (i == query.size() - 1) {
						if(score[files->file] >= 0) score[files->file] += 100;
					}
				}
			}
		}
		if (!flag) break;
	}
}
/*void SearchEngine::operator9(string query, int*& score)
{
	vector <string> wordVector = split(query.substr(1, query.length() - 2));
	vector <FileNode*> files;
	for (int i = 0; i < wordVector.size(); i++)
		files.emplace_back(root->searchFilesToScore(wordVector[i]));

	#pragma region HEAP_DECLEARATION
	typedef tuple <FileNode*, PosNode*, int> HeapNode;
	//file - pos - word idx
	auto heap_comp = [](const HeapNode& a, const HeapNode& b)
	{
		FileNode *aFile, *bFile;
		PosNode *aPos, *bPos;
		tie(aFile, aPos, ignore) = a;
		tie(bFile, bPos, ignore) = b;

		return (aFile->file > bFile->file) || (aFile->file == bFile->file && aPos > bPos);
	};
	priority_queue <HeapNode, vector <HeapNode>, decltype(heap_comp)> pq (heap_comp);
	#pragma endregion 

	for (int i = 0; i < files.size(); i++)
	{
		if (files[i] != nullptr)
			return;
		pq.push(make_tuple(files[i], files[i]->posRoot, i));
	}

	int cnt_word = 0, pre_file = 0, exp_pos;
	while (!pq.empty())
	{
		FileNode* file;
		PosNode* pos;
		int word;
		tie(file, pos, word) = pq.top();
		pq.pop();

		//Proccess continuous
		if (file->file != pre_file)
			cnt_word = 0;
		if (word == cnt_word && (!word || exp_pos == pos->pos))
		{
			++cnt_word;
			exp_pos = pos->pos + wordVector[word].size();
			if (cnt_word >= wordVector.size())
			{
				score[file->file] += wordVector.size();
				cnt_word = 0;
			}
		}
		else cnt_word = 0;
		pre_file = file->file;

		//Push next pos
		pos = pos->Next;
		if (pos != nullptr)
			pq.push(make_tuple(file, pos, word));
		else
		{
			file = file->Next;
			if (file != nullptr)
			{
				pos = file->posRoot;
				pq.push(make_tuple(file, pos, word));
			}
		}
	}
}*/



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
		score[max] *= -1;
	}
}

void SearchEngine::writeText(int i, vector<string>& queries) {
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
void printCharacter(int n, char c) {
	for (int j = 0; j < n; j++) cout << c;
}
void printCharacterColor(int n, string c, int color) {
	for (int j = 0; j < n; j++) WriteInColor(color,c);
}
int getMaxLength(vector<string>& s) {
	if (s.size() <= 0) return 0;
	int max = s[0].length();
	for (int i = 0; i < s.size(); i++) if (s[i].length() > max) max = s[i].length();
	return max%2 == 0 ? max : max+1;
}
void UI::print() {
	system("cls");
	int maxContent = getMaxLength(content);
	int maxQuery = getMaxLength(content) * 3 / 4;
	if (sub_box.size() > 0 && getMaxLength(sub_box) > maxQuery) maxQuery = getMaxLength(sub_box);
	//Print top
	cout << (char)201;
	printCharacter(offset_x * 2 + maxContent, (char)205);
	cout << (char)187<<endl;
	for (int i = 0; i < content.size() + offset_y*2; i++) {
		if ((i >= offset_y && i < content.size() + offset_y)) {
			cout << (char)186;
			printCharacter(offset_x, ' ');
			cout << content[i-offset_y];
			printCharacter(offset_x + maxContent- content[i - offset_y].length(), ' ');
			cout << (char)186 << endl;
		}
		else {
			cout << (char)186;
			printCharacter(maxContent+offset_x*2, ' ');
			cout << (char)186 << endl;
		}
	}
	//Print bottom
	cout << (char)200;
	printCharacter((offset_x * 2 + maxContent)/2-maxQuery/2-1 - offset_subbox_x, (char)205);
	cout << (char)203;
	printCharacter(maxQuery+offset_subbox_x*2, (char)205);
	cout << (char)203;
	printCharacter((offset_x * 2 + maxContent)/2-maxQuery /2-1 - offset_subbox_x, (char)205);
	cout << (char)188 << endl;
	for (int i = 0; i < sub_box.size() + offset_subbox_y * 2; i++) {
		printCharacter((offset_x * 2 + maxContent) / 2 - maxQuery / 2 - offset_subbox_x, ' ');
		if ((i >= offset_subbox_y && i < sub_box.size() + offset_subbox_y)) {
			cout << (char)186;
			printCharacterColor(offset_subbox_x, " ", k == i ? 44 : 36);
			WriteInColor(k == i ? 44 : 36,sub_box[i - offset_subbox_y]);
			printCharacterColor(offset_subbox_x + maxQuery - sub_box[i - offset_subbox_y].length(), " ", k == i ? 44 : 36);
			cout << (char)186 << endl;
		}
		else {
			cout << (char)186;
			printCharacter(maxQuery + offset_subbox_x * 2, ' ');
			cout << (char)186 << endl;
		}
	}
	printCharacter((offset_x * 2 + maxContent) / 2 - maxQuery / 2 - offset_subbox_x, ' ');
	cout << (char)200;
	printCharacter(maxQuery + offset_subbox_x * 2, (char)205);
	cout << (char)188 << endl;
}
string SearchTask::getQuery(vector<string>& s) {
	string ss = "";
	for (int i = 0; i < s.size(); i++) ss += s[i];
	return ss;
}