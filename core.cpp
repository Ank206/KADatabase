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
#include "structures.cpp"
#include "parser.cpp"
#include "tableCreator.cpp"
#include "metaReader.cpp"
#include "operations.cpp"

using namespace std;

template <typename T>
class JobQueue
{
	private:
		queue<T> queue;
		mutex mtx;
		condition_variable cv;
	public:
		void push(T item)
		{
			unique_lock<mutex> lock(mtx);
			queue.push(item);
			lock.unlock();
			cv.notify_one();
		}
		T pop()
		{
			unique_lock<mutex> lock(mtx);
			while(queue.empty())
			{
				cv.wait(lock);
			}
			T item = queue.front();
			queue.pop();
			return item;
		}
		bool empty()
		{
			lock_guard<mutex> lock(mtx);
			return queue.empty();
		}
};


struct UserRecord
{
	int id;
	char username[32];
	int age;
	bool active;
};

struct DBJob
{
	int operationType;
	UserRecord temp;
};


class SimpleDB
{
	private: 
			string filename;
			fstream file;
			unordered_map<int, long long> index;
	public:
			SimpleDB(string fname)
			{
				filename = fname;
				file.open(filename, ios::in|ios::out|ios::binary);

				if(!file.is_open())
				{
					file.open(filename, ios::out|ios::binary);
					file.close();
					file.open(filename, ios::in|ios::out|ios::binary|ios::app);
				}
				rebuildIndex();
			}
			~SimpleDB()
			{
				if(file.is_open()) file.close();
			}

			void rebuildIndex()
			{
				cout << "[System] Rebuilding Index table from Disk..." << endl;
				file.clear();
				file.seekg(0, ios::beg);

				UserRecord temp;
				long long current_position = 0;

				while(file.read((char*)& temp, sizeof(UserRecord)))
				{
					index[temp.id] = current_position;
					current_position = file.tellg();
				}

				file.clear();

				cout << "[System] Index building complete." << endl;
				cout << "Found " << index.size() << " records." << endl;
			}
			void insertUser(int id, string name, int age)
			{
				UserRecord temp;
				temp.id = id;
				strncpy(temp.username, name.c_str(), 31); 
				temp.username[31] = '\0';
				temp.age = age;
				temp.active = true;

				file.seekp(0, ios::end);
				long long writePosition = file.tellp();

				file.write((char*)& temp, sizeof(UserRecord));
				file.flush();

				index[id] = writePosition;
				cout << "[Success] Saved User: " << name << " at offset " << writePosition << endl;
			}
			void getUser(int id)
			{
				if(index.find(id) == index.end())
				{
					cout << "[Fatal Error] User ID " << id << " does not exist." << endl;
					return;
				}
				long long position = index[id];
				file.seekg(position, ios::beg);
				UserRecord temp;
				file.read((char*)& temp, sizeof(UserRecord));
				cout << endl;
				cout << "--- Record Found ---" << endl;
				cout << "ID: " << temp.id << endl;
				cout << "Name: " << temp.username << endl;
				cout << "Age: " << temp.age << endl;
				cout << "--- End of Record ---" << endl;
				cout << endl;
			}
			void getAllUser()
			{
				file.clear();
				file.seekg(0, ios::beg);
				UserRecord temp;
				vector<UserRecord> data;
				while(file.read((char*)& temp, sizeof(UserRecord)))
				{
					if(temp.active)
						data.push_back(temp);
				}
				cout << "Found " << data.size() << " records." << endl;

				for(auto i: data)
					cout << i.id << " | " << i.username << " | " << i.age << endl;
				file.clear();
			}
};

JobQueue<DBJob> workQueue;
bool systemrunning = true;
bool done = false;

void backgroundWorker(SimpleDB* db)
{
	cout << "[System]: Background thread started." << endl;
	while(systemrunning || !workQueue.empty())
	{
		DBJob job = workQueue.pop();
		if(job.operationType == -1)
		{
			systemrunning = false;
			break;
		}
		Sleep(10000);
		if(job.operationType == 1)
		{
			db -> insertUser(job.temp.id, job.temp.username, job.temp.age);
			cout << "Finished writing ID: " << job.temp.id<< endl;
		}
	}
	cout << "[System]: Background thread stopped." << endl;
	done = true;
}

int main()
{
	cout << "Welcome to KADatabase" << endl;
	SimpleDB db("my_database.dat");

	thread worker(backgroundWorker, &db);
	worker.detach();

	while(true)
	{
		string command; 
		getline(cin, command);
		commandStruct temp = parser(command);
		cout << temp.type << endl;

		if(temp.type == 0)
		{
			// request for table creation.
			tableCreator(temp);
		}
		else if(temp.type == 1)
		{
			write(temp);
		}
		else if(temp.type == 2)
		{

		}
		else
		{
			cout << "[ERROR]: Fatal Error, wrong command passed." << endl;
		}

		// cout << "\n 1. Add User  2. Get User  3. Get All Users  4. Exit" << endl;
		// cout << "Enter Choice: ";
		// int choice; cin >> choice;
		// cout << endl;

		// if(choice == 1)
		// {
		// 	int id;
		// 	int age;
		// 	string name;
		// 	cout << "Enter ID: "; cin >> id;
		// 	cin.ignore();

		// 	cout << "Enter Name: "; 
		// 	getline(cin, name);

		// 	cout << "Enter Age: "; cin >> age;
		// 	UserRecord temp;
		// 	temp.id = id;
		// 	strncpy(temp.username, name.c_str(), 31);
		// 	temp.username[31] = '\0';
		// 	temp.age = age;
		// 	workQueue.push({1, temp});
		// }
		// else if(choice == 2)
		// {
		// 	int id;
		// 	cout << "Enter ID to search: ";
		// 	cin >> id;
		// 	db.getUser(id);
		// }
		// else if(choice == 3)
		// {
		// 	db.getAllUser();
		// }
		// else
		// {
		// 	systemrunning = false;
		// 	while(!done){}
		// 	break;
		// }
	}
	return 0;
}