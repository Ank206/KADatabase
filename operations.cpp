void write(commandStruct input)
{
	string fileName = string(input.tokens[0])+".dat";
	fstream file;
	file.open(fileName, ios::in|ios::out|ios::binary);
	file.seekp(0, ios::beg);
	fileHeader fhead;
	file.read((char*)& fhead, sizeof(fileHeader));
	int totalCols = fhead.num_columns;
	vector<pair<bool, int>> cols;
	// 0: int 
	// 1: string
	for(int i = 0; i<totalCols; i++)
	{
		columnDefination colDef;
		file.read((char*)& colDef, sizeof(columnDefination));
		if(colDef.isString)
			cols.push_back({1, colDef.size});
		else
			cols.push_back({0, 4});
	}
	file.seekp(0, ios::end);
	for(int i = 0; i<totalCols; i++)
	{
		string s = input.tokens[i+2];
		int sn = -1;
		int maxLen = cols[i].second;
		if(cols[i].first == 0) sn = stoi(input.tokens[i+2]); 
		if(cols[i].first == 1)
		{
			// This solves the padding issue (e.g., "Hi" becomes "Hi\0\0\0...")
            vector<char> buffer(maxLen, 0); 

            // Copy string to buffer safely
            strncpy(buffer.data(), s.c_str(), maxLen-1);

            // FIX 3: Write the BUFFER content, not the address
            file.write(buffer.data(), maxLen);
		}
		else
			file.write((char*)& sn, cols[i].second);
	}
	file.close();
}
void indexation(map<string, map<int, string>> &main_map)
{
	vector<string> dat_files;
	string path = ".";
	for(const auto&entry : filesystem::directory_iterator(path))
	{
		if(entry.is_regular_file() && entry.path().extension() == ".dat")
		{
			dat_files.push_back(entry.path().stem().string());
		}
	}
	// cout << "Filename fetching done." << endl;
	for(auto filename: dat_files)
	{
		fstream file;
		file.open(filename + ".dat", ios::in|ios::binary);
		file.seekg(0, ios::beg);
		fileHeader fhead;
		file.read((char*)& fhead, sizeof(fileHeader));
		int total_cols = fhead.num_columns;
		int record_size = fhead.record_size;
		vector<pair<bool, int>> cols;
		int primary_column_number = -1;
		for(int i = 0; i<total_cols; i++)
		{
			columnDefination colDef;
			file.read((char*)& colDef, sizeof(columnDefination));
			if(colDef.isPrimary) primary_column_number = i;
			if(colDef.isString)
				cols.push_back({1, colDef.size});
			else
				cols.push_back({0, 4});
		}
		file.seekg(0, ios::end);
		long fileSize = file.tellg();
		long totalRecords = (fileSize - 1024)/record_size;
		file.seekg(1024, ios::beg);
		map<int, string> dat_map;
		while(totalRecords--)
		{
			// cout << "Parsing records for: " << filename << endl;
			string record_as_string = "";
			int primary_column_value = 0;
			for(int i = 0; i<cols.size(); i++)
			{
				if(cols[i].first == 0)
				{
					if(i == primary_column_number)
					{	
						file.read((char*)& primary_column_value, sizeof(int));
						record_as_string += " " + to_string(primary_column_value);	
					}
					else
					{
						int temp;
						file.read((char*)& temp, sizeof(int));
						record_as_string += " " + to_string(temp);
					}
				}
				else
				{
					int size  = cols[i].second;
					vector<char> buffer(size);
					file.read(buffer.data(), size);
					record_as_string += " " + string(buffer.data());
				}
			}
			// cout << record_as_string << endl;
			dat_map[primary_column_value] = record_as_string;
		}
		file.close();
		main_map[filename] = dat_map;
	}
}
void read(commandStruct input, map<string, map<int, string>> &main_map)
{
		string filename = input.tokens[0];
		int key = stoi(input.tokens[2]);
		cout << main_map[filename][key] << endl;
}