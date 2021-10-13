#pragma once
#include <string>
#include "Type.hpp"
#include "Token.hpp"

struct Node
{
	int column = 1;
	int line = 1;

	Type type = TYPE_EMPTY;
	std::shared_ptr<Node> left = nullptr;
	std::shared_ptr<Node> right = nullptr;

	bool is_op = false;
	bool is_post_op = false;

	bool is_p_expr = false;
	bool is_list_item = false;

	int int_value = 0;
	float float_value = 0;
	bool bool_value = 0;

	std::shared_ptr<std::string> id_value = nullptr;
	std::shared_ptr<std::string> string_value = nullptr;

	// Call
	std::shared_ptr<std::string> call_name = nullptr;
	std::vector<std::shared_ptr<Node>> args;

	// Func
	std::shared_ptr<std::string> func_name;
	std::vector<std::shared_ptr<Node>> func_params;
	std::vector<std::shared_ptr<Node>> func_body;
	std::shared_ptr<Node> return_type = nullptr;

	// Block
	//std::shared_ptr<Node> block_name = nullptr;
	std::string block_name;
	std::vector<std::shared_ptr<Node>> body;

	// If Atom
	std::shared_ptr<Node> if_else_expr = nullptr;
	std::shared_ptr<Node> if_else_body = nullptr;

	// If statements
	std::vector<std::shared_ptr<Node>> if_else_statements;

	// Ref
	std::shared_ptr<Node> ref;
	std::shared_ptr<Node>* ref_address;

	// Type
	std::string type_type = "any";

	// Var
	std::shared_ptr<Node> var_type = nullptr;

	// Type Def
	std::shared_ptr<std::string> type_name;
	std::vector<std::shared_ptr<Node>> type_body;

	// Return
	std::shared_ptr<Node> return_statement = nullptr;

	// List
	std::vector<std::shared_ptr<Node>> list_items;

	// Range
	std::vector<std::shared_ptr<Node>> range_items;

	// While
	std::shared_ptr<Node> while_expr = nullptr;
	std::vector<std::shared_ptr<Node>> while_body;

	// Scope
	std::string scope_name;
	std::shared_ptr<Node> scope_parent = nullptr;
	bool scope_is_named = false;

	std::map <std::string, std::pair<std::string, std::shared_ptr<Node>>> scope_data;
	std::vector<std::string> scope_types;

	Node() : type(TYPE_EMPTY) {}
	Node(Type type) : type(type) {}
	Node(std::shared_ptr<Token> token) : type(token->type), column(token->column), line(token->line),
		id_value(token->id_value), int_value(token->int_value), float_value(token->float_value), bool_value(token->bool_value),
		string_value(token->string_value), is_op(token->is_op), is_post_op(token->is_post_op) {}

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