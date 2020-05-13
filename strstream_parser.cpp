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

class Comment : public Token
{
public:
	friend std::istringstream &operator>> (std::istringstream &input, Comment &comment) {
		const auto start_pos = input.tellg();
		std::string temp_string;
		std::getline(input >> std::ws, temp_string);	// trims whitespace from beginning, but not the end. TODO.
		if (temp_string.front() == ';') {
			comment.setText(temp_string.trim());
		} else {
			std::cerr << "Malformed comment field: '" << temp_string << "'" << std::endl;
		}
	};
};

class Params : public Token
{
public:
	friend std::istringstream &operator>> (std::istringstream &input, Params &params) {
		const auto start_pos = input.tellg();
		bool reset_stream = true;
		std::string temp_string;
		input >> temp_string;
		switch (temp_string.front()) {
		case ';' : // it's a comment
			break;
		case '#' : // it's a immediate 
			//TODO: parse immediate value or label and return reset_stream = <true|false> whether parse succeeded
			break;
		case '(' : // it's an indirect addressing mode
			//TODO: parse indirect addressing modes
			break;
		case '$' : // it's a absolute addressing mode
			//TODO: parse absolute addressing modes
			break;
		case '-' : // it's a relative branch backwards. Note label could result in offset in Bxx
			//TODO: parse relative branch as offset
			break;
		case '+' : // it's a relative branch forward
			//TODO: parse relative branch as offset. Note label could result in offset in Bxx
			break;
		default : // could be Accumulator, absolute (with decimal number or label)
		}
		if (reset_stream) {
			input.seekg(start_pos);
		}
		return input;
	}
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
		Params params;
		Comment comment;
		std::string rest;
		std::istringstream streamline(line);
		if (streamline) {
			linenumber++;
			streamline >> label >> mnemonic >> params >> comment;
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
