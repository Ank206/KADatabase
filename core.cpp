#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <cstring>
#include <windows.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <filesystem>
#include <map>
#include "structures.cpp"
#include "parser.cpp"
#include "tableCreator.cpp"
#include "metaReader.cpp"
#include "operations.cpp"

using namespace std;
map<string, map<int, pair<int, string>>> main_map;
/*
	map<string(table_name), map<int(primary_key), pair<int(delete_status), string(query)>>> main_map;
*/

int main()
{
	cout << "Welcome to KADatabase" << endl;

	while (true)
	{
		string command;
		getline(cin, command);
		commandStruct temp = parser(command);
		indexation(main_map);
		// cout << temp.type << endl;
		if (temp.type == 0)
		{
			// request for table creation.
			tableCreator(temp);
			indexation(main_map);
		}
		else if (temp.type == 1)
		{
			write(temp, main_map);
		}
		else if (temp.type == 2	)
		{
			read(temp, main_map);
		}
		else if (temp.type == 3)
		{
			del(temp, main_map);
		}
		else
		{
			cout << "[ERROR]: Fatal Error, wrong command passed." << endl;
		}
	}
	return 0;
}