#include "Token.hpp"

const std::string Token::get_id_value()
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

int Token::get_int_value()
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

float Token::get_float_value()
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

const std::string Token::get_string_value()
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