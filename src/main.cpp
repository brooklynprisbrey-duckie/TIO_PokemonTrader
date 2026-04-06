#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

struct Pokemon {
	string species;
	int level = 0;
};

string getBasePath() {
	fs::path p = fs::current_path(); //gets current path

	while (p.has_parent_path()) { //is there a directory above it?
		if (fs::exists(p / "src")) {//is there a src file here?
			return(p / "src").string() + '/'; //then we are done looking.
		}
		p = p.parent_path();
	}
	return "src/";
}

vector<string> loadVersion(const string& filename) {
	vector<string> list;
	ifstream file(filename);

	if (!file.is_open()) {
		cout << "No file, space cowboy" << endl;
		return list;
	}
	string pmName;
	while (file >> pmName) {//reads in until first whitespace until file end.
		list.push_back(pmName);
	}

	return list;
}

vector<Pokemon> loadCaptured(const string& filename) {
	vector<Pokemon> list;
	ifstream file(filename);

	if (!file.is_open()) {
		cout << "No Pokemon, space cowboy" << endl;
		return list;
	}

	string pmName;
	int level;
	while (file >> pmName >> level) {
		list.push_back({ pmName, level });
	}
	return list;
}

void saveCaptured(const string& filename, const vector<Pokemon>& list) {
	ofstream file(filename);

	for (const auto& pm : list) {//for each loop
		file << pm.species << " " << pm.level << endl;
	}
	return;
}

void viewCaptured(const vector<Pokemon>& list) {
	if (list.empty()) {
		cout << "\nYou can't go into the tall grass! You have no Pokemon to protect you!" << endl;
		return;
	}

	cout << "\nYour Pokedex:\n";
	for (const auto& pm : list) {
		cout << pm.species << ", " << pm.level << "\n";
	}
	cout << endl;
	return;
}

void attemptCapture(vector<string>& versionList, vector<Pokemon>& capturedList) {
	if (versionList.empty()) {
		cout << "But no 'mon came." << endl;
		return;
	}

	string encounter = versionList[rand() % versionList.size()];
	cout << "You encountered a wild " << encounter << "!\n";
	
	cout << "Enter a number (1-15)" << endl;
	int guess;
	int target = rand() % 15 + 1;
	while (!(cin >> guess)) {
		cout << "A number between 1-15." << endl;
	}

	if (abs(guess - target) <= 3) {
		cout << "You captured a " << encounter << endl;

		bool found = false;
		for (auto& pm : capturedList) {
			if (pm.species == encounter) {
				if (pm.level < 5) pm.level++;
				cout << "Your " << pm.species << " leveled up to " << pm.level << "!\n";
				found = true;
				break;
			}
		}

		if (!found) {
			capturedList.push_back({ encounter, 1 });
			cout << "You discovered a new pokemon!" << endl;
		}
	}
	else {
		cout << "The wild " << encounter << " escaped!" << endl;
	}

	return;
}

void tradePokemon(vector<Pokemon>& capturedList, const string& basePath) {
	string tradePath = basePath + "trade";

	if (!fs::exists(tradePath)) {
		cout << "There's no-one to trade with.\n";
		return;
	}

	vector<string> files;

	for (const auto& entry : fs::directory_iterator(tradePath)) {//should go through entire trade file
		files.push_back(entry.path().string());
	}

	if (files.empty()) {
		cout << "Don't let your friends go into the tall grass!!\n They have no pokemon!" << endl;
		return;
	}

	cout << "\nTrade Files\n";
	for (int i = 0; i < files.size(); i++) {
		cout << i << ": " << files[i] << endl;
	}

	int choice;
	cout << "Select file: ";
	while (!(cin >> choice)) {
		cout << "Pick a number" << endl;
	}
	
	if (choice < 0 || choice >= files.size()) return;
	
	vector<Pokemon> friendList = loadCaptured(files[choice]);

	if (capturedList.empty() || friendList.empty()) {
		cout << "Look at you... a couple of monless trainers...\n"
			"Don't go into the tall grass." << endl;
		return;
	}

	cout << "\nYour Pokemon Team\n";
	for (const auto& pm : capturedList) {
		cout << pm.species << ", " << pm.level << "\n";
	}
	int myPick;
	cout << "Choose a Pokemon to fight! (Number): ";
	while (!(cin >> myPick)) {
		cout << "Pick a number" << endl;
	}

	cout << "\nTheir Pokemon Team\n";
	for (const auto& pm : friendList) {
		cout << pm.species << ", " << pm.level << "\n";
	}
	int theirPick;
	cout << "Choose a Pokemon to fight! (Number): ";
	while (!(cin >> theirPick)) {
		cout << "Pick a number" << endl;
	}

	int myPower = capturedList[myPick].level + (rand() % 6);
	int theirPower = friendList[theirPick].level + (rand() % 6);

	cout << "\nBattle Results\n";
	if (myPower >= theirPower) {
		cout << "You win and obtain " << friendList[theirPick].species << endl;

		bool found = false;
		for (auto& pm : capturedList) {
			if (pm.species == friendList[theirPick].species) {
				if (pm.level < 5) pm.level++;
				cout << "Your " << pm.species << " leveled up to " << pm.level << "!\n";
				found = true;
				break;
			}
		}

		if (!found) {
			capturedList.push_back({ friendList[theirPick].species, 1 });
			cout << "You won a new pokemon!" << endl;
		}
	}
	else {
		cout << "You lost!" << endl;
	}
	return;
}

int main() {
	srand(time(0));
	
	string base = getBasePath();

	cout << "Choose version ( red/blue ): ";
	string version;
	cin >> version;

	string versionFile =
		(version == "red") ? base + "red.txt" : base + "blue.txt";//trinary operator???

	vector<string> versionList = loadVersion(versionFile);
	vector<Pokemon> capturedList = loadCaptured(base + "captured.txt");

	int choice;

	do {
		cout << "\n1. Capture Pokemon\n"
			"2. View Pokemon\n"
			"3. Trade Pokemon\n"
			"4. Exit" << endl;
		cin >> choice;

		if (choice == 1) {
			attemptCapture(versionList, capturedList);
			saveCaptured(base + "captured.txt", capturedList);
		}
		else if (choice == 2) {
			viewCaptured(capturedList);
		}
		else if (choice == 3) {
			tradePokemon(capturedList, base);
			saveCaptured(base + "captured.txt", capturedList);
		}
	} while (choice != 4);

	return 0;
}