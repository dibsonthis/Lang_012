#pragma once

#include <string>
#include "Type.hpp"

struct Token
{
	Type type = TYPE_EMPTY;
	int line = 0;
	int column = 0;

	bool is_op = false;
	bool is_post_op = false;

	int int_value = 0;
	float float_value = 0.0f;
	bool bool_value = 0;

	std::shared_ptr<std::string> id_value = nullptr;
	std::shared_ptr<std::string> string_value = nullptr;

	Token(int lexer_line, int lexer_column) : line(lexer_line), column(lexer_column) {}
	Token(Type type, int lexer_line, int lexer_column) : type(type), line(lexer_line), column(lexer_column) {}

	const std::string get_id_value();

	int get_int_value();

	float get_float_value();

	const std::string get_string_value();
};

void print_token(std::shared_ptr<Token> token);

void print_tokens(std::vector<std::shared_ptr<Token>> tokens);