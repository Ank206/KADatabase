#include <iostream>
#include <vector>
using namespace std;

struct commandStruct
{
	int type; // 0: create, 1: add, 2: delete
	char tableName[32];
	vector<string> tokens;
};

commandStruct parser(string command)
{
	vector<string> tokens;
	string tempString = "";
	for(auto i: command)
	{
		if(i != ' ')
		{
			tempString.push_back(i);
		}
		else
		{
			tokens.push_back(tempString);
			tempString = "";
		}
	}
	if(tempString != "") tokens.push_back(tempString);

	commandStruct temp;

	if (tokens.size() < 2) {
        cout << "ERROR: Incomplete Command" << endl;
        temp.type = -1;
        return temp;
    }
	if(tokens[0] == "create")
	{
		temp.type = 0;
		strncpy(temp.tableName, tokens[1].c_str(), 31);
		temp.tableName[31] = '\0';
		temp.tokens = tokens;
	}
	else if(tokens[1] == "add")
	{
		temp.type = 1;
		strncpy(temp.tableName, tokens[0].c_str(), 31);
		temp.tableName[31] = '\0';
		temp.tokens = tokens;
	}
	else if(tokens[1] == "delete")
	{	
		temp.type = 2;
		strncpy(temp.tableName, tokens[0].c_str(), 31);
		temp.tableName[31] = '\0';
		temp.tokens = tokens;
	}
	else
	{
		cout << "ERROR: Invalid Command" << endl;
		temp.type = -1;
	}
	return temp;
}