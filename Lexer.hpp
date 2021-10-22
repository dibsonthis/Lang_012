#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <streambuf>
#include <map>
#include <iostream>

#include "Type.hpp"
#include "Token.hpp"

class Lexer
{
	std::string file_name;
	std::string source;
	int source_length;
	int index = 0;
	char current_char;
	int line = 1;
	int column = 1;
	std::vector<std::string> errors;
	bool debug = false;

	void load_source(std::string filename);

	void error_and_exit(std::string message);

	void error_and_continue(std::string message);

	void advance();

	char peek();

	void build_identifier();

	void build_number();

	void build_string();

	void handle_line_comment();

	void handle_block_comment();

public:

	std::vector<std::shared_ptr<Token>> tokens;

	Lexer(std::string src, bool is_file = true);

	void check_for_errors();

	std::string& get_file_name();

	std::vector<std::shared_ptr<Token>> get_tokens();

	std::string& get_source();

	void tokenize();
};
