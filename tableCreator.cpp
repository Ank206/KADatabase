void tableCreator(commandStruct input)
{
	// table name from input.tableName
	uint32_t cols = input.tokens.size() - 2;

	fileHeader fhead;
	strncpy(fhead.signature, "KAD", 4);
	fhead.num_columns = cols;

	int recordSize = 0;
	vector<columnDefination> colDefs;
	for (int i = 2; i < input.tokens.size(); i++)
	{
		vector<string> dataDefinations;
		string temp = "";
		for (auto j : input.tokens[i])
		{
			if (j == ':')
			{
				dataDefinations.push_back(temp);
				temp = "";
			}
			else
				temp.push_back(j);
		}
		if (temp != "")
			dataDefinations.push_back(temp);
		columnDefination column;
		strncpy(column.columnName, dataDefinations[0].c_str(), 31);
		column.columnName[31] = '\0';
		if (dataDefinations[1] == "string")
		{
			column.isString = true;
			column.size = stoi(dataDefinations[2]);
		}
		else if (dataDefinations.size() == 3)
			column.isPrimary = true;
		if (dataDefinations.size() == 4)
			column.isPrimary = true;

		recordSize += column.size;
		colDefs.push_back(column);
	}
	fhead.record_size = recordSize;
	cout << "Creation command parsed successfully." << endl;

	fstream file;
	string filename = string(input.tableName) + ".dat";
	file.open(filename, ios::trunc | ios::out | ios::binary);
	file.seekp(0, ios::end);
	file.write((char *)&fhead, sizeof(fileHeader));
	file.flush();

	for (int i = 0; i < colDefs.size(); i++)
	{
		file.seekp(0, ios::end);
		file.write((char *)&colDefs[i], sizeof(columnDefination));
		file.flush();
	}

	long currentPos = file.tellp();
	if (currentPos < 1024)
	{
		vector<char> padding(1024 - currentPos, 0);
		file.write(padding.data(), padding.size());
	}

	file.close();
}