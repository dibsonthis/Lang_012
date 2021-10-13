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

	const std::string get_id_value()
	{
		if (type == TYPE_ID)
		{
			return *id_value;
		}
		else
		{
			return "";
		}
	}

	int get_int_value()
	{
		if (type == TYPE_INT)
		{
			return int_value;
		}
		else
		{
			return 0;
		}
	}

	float get_float_value()
	{
		if (type == TYPE_FLOAT)
		{
			return float_value;
		}
		else
		{
			return 0.0f;
		}
	}

	const std::string get_string_value()
	{
		if (type == TYPE_STRING)
		{
			return *string_value;
		}
		else
		{
			return "";
		}
	}
};

void print_token(std::shared_ptr<Token> token)
{
	if (token->type == TYPE_ID)
	{
		std::cout << type_repr(token->type) << "(" << token->get_id_value() << ")";
	}
	else if (token->type == TYPE_INT)
	{
		std::cout << type_repr(token->type) << "(" << token->get_int_value() << ")";
	}
	else if (token->type == TYPE_FLOAT)
	{
		std::cout << type_repr(token->type) << "(" << token->get_float_value() << ")";
	}
	else if (token->type == TYPE_STRING)
	{
		std::cout << type_repr(token->type) << "("
			<< "\"" << token->get_string_value() << "\"" << ")";
	}
	else
	{
		std::cout << "TOKEN(" << type_repr(token->type) << ")";
	}
}

void print_tokens(std::vector<std::shared_ptr<Token>> tokens)
{
	std::cout << "\nTokens: " << std::to_string(tokens.size());
	std::cout << "\n[ ";
	for (auto token : tokens)
	{
		print_token(token);
		std::cout << " ";
	}
	std::cout << " ]";
}