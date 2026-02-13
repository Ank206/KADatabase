void write(commandStruct input,map<string, map<int, pair<int, string>>> &main_map)
{
	string fileName = string(input.tokens[0]) + ".dat";
	fstream file;
	file.open(fileName, ios::in | ios::out | ios::binary);
	file.seekp(0, ios::beg);
	fileHeader fhead;
	file.read((char *)&fhead, sizeof(fileHeader));
	int totalCols = fhead.num_columns;
	vector<pair<int, pair<bool, int>>> cols;
	// the first int of the pair is for primary key;
	// 0: int
	// 1: string
	for (int i = 0; i < totalCols; i++)
	{
		columnDefination colDef;
		file.read((char *)&colDef, sizeof(columnDefination));
		if(colDef.isPrimary)
		{
			if (colDef.isString)
				cols.push_back({1, {1, colDef.size}});
			else
				cols.push_back({1, {0, 4}});
		}
		else
		{
			if (colDef.isString)
				cols.push_back({0, {1, colDef.size}});
			else
				cols.push_back({0, {0, 4}});
		}
	}
	file.seekp(0, ios::end);
	int primary_column_val = -1;
	for (int i = 0; i < totalCols; i++)
	{
		string s = input.tokens[i + 2];
		int sn = -1;
		int maxLen = cols[i].second.second;
		if(cols[i].first == 1)
		{
			primary_column_val = stoi(input.tokens[i + 2]);
		}
		if (cols[i].second.first == 0)
			sn = stoi(input.tokens[i + 2]);
		if (cols[i].second.first == 1)
		{
			// This solves the padding issue (e.g., "Hi" becomes "Hi\0\0\0...")
			vector<char> buffer(maxLen, 0);

			// Copy string to buffer safely
			strncpy(buffer.data(), s.c_str(), maxLen - 1);

			// FIX 3: Write the BUFFER content, not the address
			file.write(buffer.data(), maxLen);
		}
		else
			file.write((char *)&sn, cols[i].second.second);
	}
	file.close();
	string answer = "";
	for (int i = 2; i<input.tokens.size(); i++)
	{
		answer += input.tokens[i] + " ";
	}
	main_map[string(input.tokens[0])][primary_column_val] = {0, answer};
	return;
}
void indexation(map<string, map<int, pair<int, string>>> &main_map)
{
	vector<string> dat_files;
	string path = ".";
	for (const auto &entry : filesystem::directory_iterator(path))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".dat")
		{
			dat_files.push_back(entry.path().stem().string());
		}
	}
	// cout << "Filename fetching done." << endl;
	for (auto filename : dat_files)
	{
		fstream file;
		file.open(filename + ".dat", ios::in | ios::binary);
		file.seekg(0, ios::beg);
		fileHeader fhead;
		file.read((char *)&fhead, sizeof(fileHeader));
		int total_cols = fhead.num_columns;
		int record_size = fhead.record_size;
		vector<pair<bool, int>> cols;
		int primary_column_number = -1;
		for (int i = 0; i < total_cols; i++)
		{
			columnDefination colDef;
			file.read((char *)&colDef, sizeof(columnDefination));
			if (colDef.isPrimary)
				primary_column_number = i;
			if (colDef.isString)
				cols.push_back({1, colDef.size});
			else
				cols.push_back({0, 4});
		}
		file.seekg(0, ios::end);
		long fileSize = file.tellg();
		long totalRecords = (fileSize - 1024) / record_size;
		file.seekg(1024, ios::beg);
		map<int, pair<int, string>> dat_map;
		while (totalRecords--)
		{
			// cout << "Parsing records for: " << filename << endl;
			string record_as_string = "";
			int primary_column_value = 0;
			for (int i = 0; i < cols.size(); i++)
			{
				if (cols[i].first == 0)
				{
					if (i == primary_column_number)
					{
						file.read((char *)&primary_column_value, sizeof(int));
						record_as_string += " " + to_string(primary_column_value);
					}
					else
					{
						int temp;
						file.read((char *)&temp, sizeof(int));
						record_as_string += " " + to_string(temp);
					}
				}
				else
				{
					int size = cols[i].second;
					vector<char> buffer(size);
					file.read(buffer.data(), size);
					record_as_string += " " + string(buffer.data());
				}
			}
			// cout << record_as_string << endl;
			dat_map[primary_column_value] = {0, record_as_string};
		}
		file.close();
		main_map[filename] = dat_map;
	}
}
void read(commandStruct input, map<string, map<int, pair<int, string>>> &main_map)
{
	string filename = input.tokens[0];
	int key = stoi(input.tokens[2]);
	int deleted = main_map[filename][key].first;
	string query=main_map[filename][key].second;
	if(deleted == 1)
		cout << "No entry for this key." << endl;
	else
		cout << query << endl;
}
void del(commandStruct input, map<string, map<int, pair<int, string>>> &main_map)
{
	// We will write the data in the table_name_del.dat file
	string tableName = input.tokens[0];
	int key = stoi(input.tokens[2]);
	string fileName = tableName + "_del.dat";
	fstream file;
	file.open(fileName, ios::in | ios::out | ios::binary);
	file.seekp(0, ios::end);
	file.write((char *)&key, sizeof(int));
	file.close();

	// Safety checks, so we don't accidentally keys, that don't even exsist.
	if(main_map[tableName].find(key) == main_map[tableName].end())
	{
		cout << "Key value doesn't exist in the table." << endl;
		return;
	}

	main_map[tableName][key].first = 1;
	cout << "Key value: " << key << " deleted from " << tableName << "." << endl;
}