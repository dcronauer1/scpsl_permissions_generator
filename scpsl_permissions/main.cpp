#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

/*
* go to a keyword in the file. used to go to roles list and permissions list
* returns # of lines after current line number found at
* else -1
*/
int goToString(fstream& myfile, string text) {
	int offset = 0, counter = 0;
	string line;
	while (!myfile.eof()) {
		getline(myfile, line);
		counter++;
		if ((offset = line.find(text, 0)) != string::npos) {
			cout << "found: " << text << endl;
			return counter;
		}
	}
	return -1;//didnt find
}

/*
* get the roles list
* return as a vector string
*/
vector<string> getList(fstream& myfile) {
	string line;
	int offset = 0;
	vector<string> list_vec;
	while (!myfile.eof()) {
		getline(myfile, line);
		if ((offset = line.find("-", 0)) != string::npos) {
			//grab everything after offset in line, exclude spaces and other null characters
			line = line.substr(offset + 1);
			line.erase(0, line.find_first_not_of(" \t")); // Trim leading spaces
			line.erase(line.find_last_not_of(" \t") + 1); // Trim trailing spaces
			if (!line.empty()) //only add non-empty roles
				list_vec.push_back(line);
		}
		else
			return list_vec;
	}
	cerr << "reached end of file early" << endl;
	return list_vec;//should not reach this
}

/*
* sub function of doPermissions()
* take entered 1's and 0's, and format it into a comma seperated list of roles in string form
*/
string makeChoices(vector<string> roles, string choices) {
	string options;
	bool added1stOption = false,didSomething=false;
	for (size_t i = 0; i < roles.size(); ++i) {
		if (choices[i] == '0') {//dont add
			didSomething=true;
		}
		else if (choices[i] == '1') {//add
			if (added1stOption)
				options += ", ";
			else
				added1stOption = true;
			options += roles[i];
		}
		else {
			if (!added1stOption && !didSomething)
				return "-1";
			break;
		}
	}
	return options;
}

/*
function to let user choose which permissons are granted to what roles.
Goes through each permission in config_remoteadmin.txt
returns permissions block as a string
*/
string doPermissions(fstream& myfile, vector<string> roles) {
	string permissions, line,choices,temp, userinput;
	size_t offset = 0, pos = 0;
	vector<string> list_vec;
	bool choiceCorrect=false;

	cout << "change permission values. input a string of 0's and 1's for each role if the perm should be given or not." << endl;
	cout << "Role Order :\n";
	for (int i = 0; i < roles.size(); ++i) {
		cout << roles[i] << ",";
	}
	cout << endl;
	while (!myfile.eof()) {
		getline(myfile, line);

		if ((offset = line.find("-", 0)) != string::npos) {
			pos = line.find("["); // Find the first occurrence of the character
			if (pos != string::npos) {
				cout << line << endl;
				temp = line;
				temp.erase(pos);
				choiceCorrect = false;
				while (!choiceCorrect) {
					cout << temp << "values (2 to skip): ";
					cin >> choices;
					if (choices == "2"){
						break;//skip
					}
					else {
						choices = makeChoices(roles, choices);//format the options into a string
						if (choices != "-1") {
							cout << "is: " << choices << " correct? (yes=1)" << endl;
							cin >> userinput;
							choiceCorrect = (userinput == "1");
						}
						else {cout << "Bad entry, ";}
					}
				}
				if (choiceCorrect) {//if no change, then this shouldnt be entered
					//modify the line
					line.erase(pos + 1);//del everything after "["
					line += choices + "]";
				}
			}
			permissions += line+"\n";
		}
		else
			return permissions;
	}
	std::cerr << "reached end of file early" << endl;
	return permissions;//should not reach this
}

int main() {
	string filepath, line;
	cout << "input file path: ";
	cin >> filepath;
	//filepath = "C:\\Users\\Dom\\Desktop\\config_remoteadmin.txt";
	fstream myfile(filepath);
	if (myfile.is_open()) {
		vector<string> roles, permissions;
		int linenum = 0;//to get the 
		if ((goToString(myfile, "Roles:")) >= 0) {//find roles to then get all role names

			roles = getList(myfile);//get roles & stop when all found

			//go to permissions
			if ((goToString(myfile, "Permissions:")) >= 0) {//find Permissions
				string permissions = doPermissions(myfile, roles);//get permissions & stop when all found
				cout << endl<<permissions<<endl;//temp
				cout << "enter two strings to finish";
				cin >> filepath;
				cin >> filepath;
			}
		}
	}

	else std::cerr << "Error opening file: " << filepath << endl;
	myfile.close();
	return 0;
}