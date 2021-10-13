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

	void load_source(std::string filename)
	{
		std::ifstream stream(filename);
		std::string source_str((std::istreambuf_iterator<char>(stream)),
			std::istreambuf_iterator<char>());
		source = source_str;
	}

	void error_and_exit(std::string message)
	{
		std::string error_message = "\n\n[Lexer] Lexical Error in '" + file_name + "' @ (" + std::to_string(line) + ", " + std::to_string(column) + "): " + message;
		std::cout << error_message;
		std::cout << "\nCompilation failed. Press any key to exit...";
		std::cin.get();
		exit(1);
	}

	void error_and_continue(std::string message)
	{
		std::string error_message = "[Lexer] Lexical Error in '" + file_name + "' @ (" + std::to_string(line) + ", " + std::to_string(column) + "): " + message;
		errors.push_back(error_message);
	}

	void advance()
	{
		index++;
		column++;
		if (index >= source_length)
		{
			current_char = '\0';
		}
		else
		{
			current_char = source[index];
		}
	}

	char peek()
	{
		int peek_index = index + 1;
		if (peek_index >= source_length)
		{
			return '\0';
		}
		else
		{
			return source[peek_index];
		}
	}

	void build_identifier()
	{
		auto token = std::make_shared<Token>(TYPE_ID, line, column);

		std::shared_ptr<std::string> name = std::make_shared<std::string>();

		while (isalpha(current_char) || current_char == '_' || isdigit(current_char))
		{
			*name += current_char;
			advance();
		}

		if (*name == "true")
		{
			token->type = TYPE_BOOL;
			token->bool_value = true;
		}

		else if (*name == "false")
		{
			token->type = TYPE_BOOL;
			token->bool_value = false;
		}
		else
		{
			token->id_value = name;
		}

		tokens.push_back(token);
	}

	void build_number()
	{
		auto token = std::make_shared<Token>(line, column);

		std::string value;
		int num_dots = 0;

		while (isdigit(current_char) || current_char == '.')
		{
			value += current_char;

			if (current_char == '.')
			{
				num_dots++;
			}

			advance();
		}

		if (num_dots == 0)
		{
			token->type = TYPE_INT;
			token->int_value = std::stoi(value);
		}
		else if (num_dots == 1)
		{
			token->type = TYPE_FLOAT;
			token->float_value = std::stof(value);
		}
		else
		{
			token->type = TYPE_ERROR;
			error_and_continue("Unacceptable number of dots in number token.");
		}

		tokens.push_back(token);
	}

	void build_string()
	{
		auto token = std::make_shared<Token>(TYPE_STRING, line, column);

		std::shared_ptr<std::string> str = std::make_shared<std::string>();

		advance();

		while (current_char != '"')
		{
			if (current_char == '\0')
			{
				error_and_continue("Warning: Missing end '\"', end of file reached.");
				return;
			}

			if (current_char == '\n')
			{
				line++;
				column = 0;
				advance(); // consume '\n'
			}

			*str += current_char;
			advance();
		}

		advance();

		token->string_value = std::make_shared<std::string>();

		for (int i = 0; i < (*str).length(); i++)
		{
			if ((*str)[i] == '\\' && (*str)[i + 1] == 'n')
			{
				(*str)[i] = '\n';
				(*token->string_value).push_back('\n');
				i++;
			}
			else if ((*str)[i] == '\\' && (*str)[i + 1] == 'r')
			{
				(*str)[i] = '\r';
				(*token->string_value).push_back('\r');
				i++;
			}
			else if ((*str)[i] == '\\' && (*str)[i + 1] == 't')
			{
				(*str)[i] = '\t';
				(*token->string_value).push_back('\t');
				i++;
			}
			else
			{
				(*token->string_value).push_back((*str)[i]);
			}
		}

		tokens.push_back(token);
	}

	void handle_line_comment()
	{
		advance();
		advance();

		while (current_char != '\n')
		{
			if (current_char == '\0')
			{
				return;
			}

			advance();
		}

		line++;
		column = 0;

		advance(); // consume '\n'

		return;
	}

	void handle_block_comment()
	{
		advance(); // consume '/'
		advance(); // consume '*'

		while (!(current_char == '*' && peek() == '/'))
		{
			if (current_char == '\n')
			{
				line++;
				column = 0;
			}
			if (current_char == '\0')
			{
				error_and_continue("Warning: No end to block comment, end of file reached.");
				return;
			}
			if (current_char == '/' && peek() == '*')
			{
				handle_block_comment();
			}

			advance();
		}

		advance(); // consume '*'
		advance(); // consume '/'

		return;
	}

public:

	std::vector<std::shared_ptr<Token>> tokens;

	Lexer(std::string src, bool is_file = true)
	{
		if (is_file)
		{
			load_source(src);
			file_name = src;
		}
		else
		{
			source = src;
			file_name = "stdin";
		}

		source_length = source.length();
		index = 0;
		current_char = source[0];
		line = 1;
		column = 1;
	}

	void check_for_errors()
	{
		if (errors.size() > 0)
		{
			std::cout << "[Lexer] Lexing unsuccessful - " + std::to_string(errors.size()) + " error(s) found.\n";
			for (std::string error_message : errors)
			{
				std::cout << error_message << '\n';
			}
		}
		else
		{
			if (debug)
				std::cout << "[Lexer] Lexing successful - no errors found.\n";
		}
	}

	std::string& get_file_name()
	{
		return file_name;
	}

	std::vector<std::shared_ptr<Token>> get_tokens()
	{
		return tokens;
	}

	std::string& get_source()
	{
		return source;
	}

	void tokenize()
	{
		while (current_char != '\0')
		{
			if (current_char == '\n')
			{
				column = 0;
				line++;
				advance();
			}
			else if (current_char == ' ' || current_char == '\t')
			{
				advance();
			}
			else if (isalpha(current_char) || current_char == '_')
			{
				build_identifier();
			}
			else if (isdigit(current_char))
			{
				build_number();
			}
			else if (current_char == '/' && peek() == '/')
			{
				handle_line_comment();
			}
			else if (current_char == '/' && peek() == '*')
			{
				handle_block_comment();
			}
			else if (current_char == '"')
			{
				build_string();
			}
			else if (current_char == '=' && peek() == '=')
			{
				auto token = std::make_shared<Token>(TYPE_EQ_EQ, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume =
				advance(); // consume =
			}
			else if (current_char == '!' && peek() == '=')
			{
				auto token = std::make_shared<Token>(TYPE_NOT_EQUAL, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume !
				advance(); // consume =
			}
			else if (current_char == '<' && peek() == '=')
			{
				auto token = std::make_shared<Token>(TYPE_LT_EQUAL, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume <
				advance(); // consume =
			}
			else if (current_char == '>' && peek() == '=')
			{
				auto token = std::make_shared<Token>(TYPE_GT_EQUAL, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume >
				advance(); // consume =
			}
			else if (current_char == '+' && peek() == '=')
			{
				auto token = std::make_shared<Token>(TYPE_PLUS_EQ, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume +
				advance(); // consume =
			}
			else if (current_char == '-' && peek() == '=')
			{
				auto token = std::make_shared<Token>(TYPE_MINUS_EQ, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume -
				advance(); // consume =
			}
			else if (current_char == '+' && peek() == '+')
			{
				auto token = std::make_shared<Token>(TYPE_PLUS_PLUS, line, column);
				token->is_post_op = true;
				tokens.push_back(token);
				advance(); // consume +
				advance(); // consume +
			}
			else if (current_char == '-' && peek() == '-')
			{
				auto token = std::make_shared<Token>(TYPE_MINUS_MINUS, line, column);
				token->is_post_op = true;
				tokens.push_back(token);
				advance(); // consume -
				advance(); // consume -
			}
			else if (current_char == ':' && peek() == ':')
			{
				auto token = std::make_shared<Token>(TYPE_DOUBLE_COLON, line, column);
				tokens.push_back(token);
				advance(); // consume :
				advance(); // consume :
			}
			else if (current_char == '=' && peek() == '>')
			{
				auto token = std::make_shared<Token>(TYPE_RIGHT_ARROW, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume =
				advance(); // consume >
			}
			else if (current_char == '-' && peek() == '>')
			{
				auto token = std::make_shared<Token>(TYPE_RIGHT_ARROW_SINGLE, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume -
				advance(); // consume >
			}
			else if (current_char == '&' && peek() == '&')
			{
				auto token = std::make_shared<Token>(TYPE_AND, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume &
				advance(); // consume &
			}
			else if (current_char == '|' && peek() == '|')
			{
				auto token = std::make_shared<Token>(TYPE_OR, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume |
				advance(); // consume |
			}
			else if (current_char == '=' && peek() == '&')
			{
				auto token = std::make_shared<Token>(TYPE_EQ_AND, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume =
				advance(); // consume &
			}
			else if (current_char == '=')
			{
				auto token = std::make_shared<Token>(TYPE_EQUAL, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '(')
			{
				auto token = std::make_shared<Token>(TYPE_LPAREN, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == ')')
			{
				auto token = std::make_shared<Token>(TYPE_RPAREN, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '{')
			{
				auto token = std::make_shared<Token>(TYPE_LBRACE, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '}')
			{
				auto token = std::make_shared<Token>(TYPE_RBRACE, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '[')
			{
				auto token = std::make_shared<Token>(TYPE_LBRACKET, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == ']')
			{
				auto token = std::make_shared<Token>(TYPE_RBRACKET, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '<')
			{
				auto token = std::make_shared<Token>(TYPE_LANGLE, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '>')
			{
				auto token = std::make_shared<Token>(TYPE_RANGLE, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '.')
			{
				auto token = std::make_shared<Token>(TYPE_DOT, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '\\')
			{
				auto token = std::make_shared<Token>(TYPE_BACKSLASH, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '\'')
			{
				auto token = std::make_shared<Token>(TYPE_APOSTROPHE, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '!')
			{
				auto token = std::make_shared<Token>(TYPE_EXCLAMATION, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '@')
			{
				auto token = std::make_shared<Token>(TYPE_AT, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '#')
			{
				auto token = std::make_shared<Token>(TYPE_HASH, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '$')
			{
				auto token = std::make_shared<Token>(TYPE_DOLLAR, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '^')
			{
				auto token = std::make_shared<Token>(TYPE_CARET, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '&')
			{
				auto token = std::make_shared<Token>(TYPE_B_AND, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '|')
			{
			auto token = std::make_shared<Token>(TYPE_B_OR, line, column);
			tokens.push_back(token);
			advance(); // consume symbol
			}
			else if (current_char == '?')
			{
				auto token = std::make_shared<Token>(TYPE_QUESTION, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '%')
			{
				auto token = std::make_shared<Token>(TYPE_PERCENT, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '"')
			{
				auto token = std::make_shared<Token>(TYPE_DOUBLE_QUOTE, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '-')
			{
				auto token = std::make_shared<Token>(TYPE_MINUS, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '+')
			{
				auto token = std::make_shared<Token>(TYPE_PLUS, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '/')
			{
				auto token = std::make_shared<Token>(TYPE_SLASH, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == '*')
			{
				auto token = std::make_shared<Token>(TYPE_STAR, line, column);
				token->is_op = true;
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == ',')
			{
				auto token = std::make_shared<Token>(TYPE_COMMA, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == ':')
			{
				auto token = std::make_shared<Token>(TYPE_COLON, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else if (current_char == ';')
			{
				auto token = std::make_shared<Token>(TYPE_SEMICOLON, line, column);
				tokens.push_back(token);
				advance(); // consume symbol
			}
			else
			{
				error_and_continue("Unexpected token '" + std::string(1, current_char) + "'.");
				auto token = std::make_shared<Token>(TYPE_ERROR, line, column);
				tokens.push_back(token);
				advance();
			}
		}

		auto token = std::make_shared<Token>(TYPE_EOF, line, column);
		tokens.push_back(token);

		check_for_errors();
	}
};
