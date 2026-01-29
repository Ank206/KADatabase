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
}
void read(commandStruct input)
{
	
}