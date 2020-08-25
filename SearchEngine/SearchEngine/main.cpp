#include "SearchEngine.h"

int main() {
	ifstream in{ "../Search_Engine/dataList.txt" };
	SearchEngine se;
	se.input(in);

	string key;
	while (1) {
		cout << "What do you want to search? ('exit' to exit) ";
		cin >> key;
		if (key == "exit")
			break;
		system("cls");
		se.search(key);
	}
	return 0;
}