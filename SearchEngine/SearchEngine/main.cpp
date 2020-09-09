#include "SearchEngine.h"
#include "iostream"
#include "string"

int* score = new int[11268]{ 0 };

int main() {
	ifstream in;
    SearchEngine se;

    start();
    in.open(WORKPLACE + "___index.txt");
	se.loadDataList(in);
	in.close();

	//for (int i = 0; i<se.searchEngineNumOfDataFiles; i++) cout << se.dataList[i] << endl;

    se.input_stop_words(WORKPLACE + "stopWords.txt");
	se.input();

	//cout << TrieNode::numTrieNode << endl;
	
	string key;

	while (1) {
		cout << "What do you want to search? ('exit' to exit) ";
		getline(cin, key);
		if (key == "exit")
			break;
        start();
		se.search(key, score);

//		cout << key << endl;
//		vector<Word> word = se.breakDown(key);
//
//		//se.search(key);
	}
	return 0;
}