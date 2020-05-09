#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <algorithm>

enum mnemonic {LDA, BNE, CMP, AND, RTS, BRK};
static std::unordered_map<std::string, enum mnemonic> mnemonic_map;

static void toUpperCase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

class Token {
protected:
	// line number?
	std::string m_string;
	bool field_present;
public:
	Token() : m_string(), field_present(false) {};
	const std::string& text() {return m_string;} const
	void setText(std::string text) {m_string = text; field_present = true;}
	operator bool() {return field_present;}
};

class Label : public Token
{
public:
	friend std::istringstream &operator>> (std::istringstream &input, Label &label) {
		const auto start_pos = input.tellg();
		std::string temp_string;
		input >> temp_string;
		// a label is a string that starts with a letter and ends with ':'
		// and is implicitly non-empty
		if (isalpha(temp_string.front()) && temp_string.back() == ':') {
			label.setText(temp_string);
		} else {
			input.seekg(start_pos);	// reset the read position
		}
		return input;
	};
	friend std::ostream &operator<< (std::ostream& output, Label &label) {
		output << "[at address] " << label.text();
		return output;
	};
};

class Mnemonic : public Token
{
public:
	friend std::istringstream &operator>> (std::istringstream &input, Mnemonic &mnemonic) {
		const auto start_pos = input.tellg();
		std::string temp_string;
		input >> temp_string;
		toUpperCase(temp_string);
		const auto iter = mnemonic_map.find(temp_string);
		const auto emptyIter = mnemonic_map.end();
		if (iter != emptyIter) {
			mnemonic.setText(iter->first); // TODO: second);
		} else {
			input.seekg(start_pos);
		}
		return input;
	};
	friend std::ostream& operator<< (std::ostream& output, Mnemonic &mnemonic) {
		output << "[at address] " << mnemonic.text();
		return output;
	};
};


int main(const int argc, const char *const argv[])
{
	mnemonic_map.insert({{"LDA", LDA},
        {"BNE", BNE},
        {"CMP", CMP},
        {"AND", AND},
        {"RTS", RTS},
        {"BRK", BRK}});

	if (argc != 2) {
		std::cerr << "Usage: " << argv[0U] << " <textfile>" << std::endl;
		return -1;
	}

	std::ifstream file(argv[1U]);
	if (!file) {
		std::cerr << "Could not open <" << argv[1U] << ">" << std::endl;
		return -2;
	}
	std::string line;
	auto linenumber = 0U;
	while (std::getline(file, line)) {
		Label label;
		Mnemonic mnemonic;
		std::string rest;
		std::istringstream streamline(line);
		if (streamline) {
			linenumber++;
			streamline >> label >> mnemonic; // TODO >> rest;
			if (label) {
				std::cout << "Label found: '" << label << "'" << std::endl;
			}
			if (mnemonic) {
				std::cout << "Mnemonic found: '" << mnemonic << "'" << std::endl;
			}
			std::cout << "The rest: '";
			while ((streamline >> rest) && (!rest.empty())) {
				std::cout << rest << std::endl;
			}
			std::cout << "'" << std::endl;
		}
	}
	return 0;
}
