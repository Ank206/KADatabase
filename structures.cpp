using namespace std;
struct commandStruct
{
	uint32_t type; // 0: create, 1: add, 2: get
	char tableName[32];
	vector<string> tokens;
};
struct fileHeader
{
	char signature[4];
	uint32_t num_columns;
	uint32_t record_size;
};
// fileheader size: 4 + 4 + 4 = 12 bytes
struct columnDefination
{
	char columnName[32];
	bool isString = false;	// then it's an int
	bool isPrimary = false; // for primary key
	uint32_t size = 4;		// 4 for int and rest for the String size.
};
// columnDefination size: 32 + 1 + 1 + 4 = 38 bytes