#include "SearchEngine.h"
#include "iostream"
#include "string"

int main() {
	ifstream in{ "../Search_Engine/dataList.txt" };
	SearchEngine se;
	se.input(in);
	
	string key;

	while (1) {
		cout << "What do you want to search? ('exit' to exit) ";
		getline(cin, key);
		if (key == "exit")
			break;
		system("cls");
		cout << key << endl;
		vector<Word> word = se.breakDown(key);
		
		//se.search(key);
	}
	return 0;
}