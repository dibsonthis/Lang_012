#pragma once

#include "Parser.hpp"
#include "Type.hpp"

std::string not_implemented_error(std::shared_ptr<Node>& op)
{
	if (op->left == nullptr)
	{
		return "Cannot perform '" + type_repr(op->type) + "' on '" + type_repr(op->right->type) + "'.";
	}

	return "Cannot perform '" + type_repr(op->type) + "' on '" + type_repr(op->left->type) + "' and '" + type_repr(op->right->type) + "'.";
}

struct Variable
{
	std::string id;
	std::shared_ptr<Node> scope;
	bool not_found = false;

	Variable() = default;
};

class Eval
{
public:

	std::string file_name = "stdin";
	std::vector<std::string> errors;

	int __scopes_num = 0;

	Eval() = default;

	Eval(Parser parser) : file_name(parser.file_name) {}

	std::shared_ptr<Node> global_scope = std::make_shared<Node>(TYPE_SCOPE);
	std::shared_ptr<Node>& current_scope = global_scope;

	std::string log_error(std::shared_ptr<Node> error_node, std::string message)
	{
		std::string error_message = "[Eval] Evaluation Error in '" + file_name + "' @ (" + std::to_string(error_node->line) + ", " + std::to_string(error_node->column) + "): " + message;
		errors.push_back(error_message);
		return error_message;
	}

	void init()
	{
		global_scope->scope_name = "global";
		global_scope->scope_types =
		{
			"int", "bool", "float", "string", "scope", "any"
		};
	}

	void eval(std::shared_ptr<Node>& node)
	{
		if (!node)
		{
			node = std::make_shared<Node>(TYPE_ERROR);
			return;
		}

		switch (node->type)
		{
			case TYPE_INT:
				node->type_type = "int";
				return;
			case TYPE_FLOAT:
				node->type_type = "float";
				return;
			case TYPE_BOOL:
				node->type_type = "bool";
				return;
			case TYPE_STRING:
				node->type_type = "string";
				return;
			case TYPE_LIST:
				node->type_type = "list";
				return;
			case TYPE_TYPE:
				node->type_type = "type";
				return;
			case TYPE_ID:
				eval_id(node);
				return;
			case TYPE_PLUS:
				eval_plus(node);
				return;
			case TYPE_MINUS:
				eval_minus(node);
				return;
			case TYPE_STAR:
				eval_mul(node);
				return;
			case TYPE_SLASH:
				eval_div(node);
				return;
			case TYPE_NEG:
				eval_neg(node);
				return;
			case TYPE_POS:
				eval_pos(node);
				return;
			case TYPE_EQUAL:
				eval_assignment(node);
				return;
			case TYPE_BLOCK:
				eval_block(node);
				return;
			case TYPE_DOUBLE_COLON:
				eval_scope_accessor(node);
				return;
			case TYPE_COLON:
				eval_type_assignment(node);
				return;
			case TYPE_CALL:
				eval_call(node);
				return;
		}
	}

	// ########### PLUS ########### //

	void eval_plus(std::shared_ptr<Node>& node)
	{
		eval(node->left);
		eval(node->right);

		//---- ERROR ----//

		if (node->left->type == TYPE_ERROR || node->right->type == TYPE_ERROR)
		{
			std::cout << "\n" << log_error(node, "Malformed '" + type_repr(node->type) + "' statement.");
			node->type = TYPE_ERROR;
			return;
		}

		//---- INT ----//

		// plus int, int
		if (node->left->type == TYPE_INT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->int_value + node->right->int_value;
		}
		// plus int, float
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->int_value + node->right->float_value;
		}
		// plus int, bool
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->int_value + node->right->bool_value;
		}

		//---- FLOAT ----//

		// plus float, int
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value + node->right->int_value;
		}
		// plus float, float
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value + node->right->float_value;
		}
		// plus float, bool
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value + node->right->bool_value;
		}

		//---- LIST ----//

		// plus list, list
		else if (node->left->type == TYPE_LIST && node->right->type == TYPE_LIST)
		{
			node->type = TYPE_LIST;
			for (auto& item : node->left->list_items)
			{
				node->list_items.push_back(item);
			}
			for (auto& item : node->right->list_items)
			{
				node->list_items.push_back(item);
			}
		}

		//---- STRING ----//

		// plus string, string
		else if (node->left->type == TYPE_STRING && node->right->type == TYPE_STRING)
		{
			node->type = TYPE_STRING;
			std::shared_ptr<std::string> string = std::make_shared<std::string>();
			*string = *node->left->string_value + *node->right->string_value;
			node->string_value = string;
		}

		//---- BOOL ----//

		// plus bool, int
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->bool_value + node->right->int_value;
		}
		// plus bool, float
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->bool_value + node->right->float_value;
		}
		// plus bool, bool
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->bool_value + node->right->bool_value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### MINUS ########### //

	void eval_minus(std::shared_ptr<Node>& node)
	{
		eval(node->left);
		eval(node->right);

		//---- ERROR ----//

		if (node->left->type == TYPE_ERROR || node->right->type == TYPE_ERROR)
		{
			std::cout << "\n" << log_error(node, "Malformed '" + type_repr(node->type) + "' statement.");
			node->type = TYPE_ERROR;
			return;
		}

		//---- INT ----//

		// minus int, int
		if (node->left->type == TYPE_INT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->int_value - node->right->int_value;
		}
		// minus int, float
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->int_value - node->right->float_value;
		}
		// minus int, string
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_STRING)
		{
			node->type = TYPE_STRING;

			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			int str_len = (*node->right->string_value).size();

			if (node->left->int_value > str_len)
			{
				node->type = TYPE_ERROR;
				return;
			}

			for (int i = node->left->int_value; i < str_len; i++)
			{
				*string = *string + (*node->right->string_value)[i];
			}

			node->string_value = string;
		}
		// minus int, bool
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->int_value - node->right->bool_value;
		}

		//---- FLOAT ----//

		// minus float, int
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value - node->right->int_value;
		}
		// minus float, float
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value - node->right->float_value;
		}
		// minus float, bool
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value - node->right->bool_value;
		}

		//---- LIST ----//

		//---- STRING ----//

		// minus string, int
		else if (node->left->type == TYPE_STRING && node->right->type == TYPE_INT)
		{
			node->type = TYPE_STRING;

			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			int str_len = (*node->left->string_value).size() - node->right->int_value;

			if (str_len < 0)
			{
				node->type = TYPE_ERROR;
				return;
			}

			for (int i = 0; i < str_len; i++)
			{
				*string = *string + (*node->left->string_value)[i];
			}

			node->string_value = string;
		}

		//---- BOOL ----//

		// minus bool, int
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->bool_value - node->right->int_value;
		}
		// minus bool, float
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->bool_value - node->right->float_value;
		}
		// minus bool, bool
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->bool_value - node->right->bool_value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}

	}

	void eval_mul(std::shared_ptr<Node>& node)
	{
		eval(node->left);
		eval(node->right);

		//---- ERROR ----//

		if (node->left->type == TYPE_ERROR || node->right->type == TYPE_ERROR)
		{
			std::cout << "\n" << log_error(node, "Malformed '" + type_repr(node->type) + "' statement.");
			node->type = TYPE_ERROR;
			return;
		}

		//---- INT ----//

		// mul int, int
		if (node->left->type == TYPE_INT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->int_value * node->right->int_value;
		}
		// mul int, float
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->int_value * node->right->float_value;
		}
		// mul int, list
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_LIST)
		{
			node->type = TYPE_LIST;
			for (int i = 0; i < node->left->int_value; i++)
			{
				for (auto& item : node->right->list_items)
				{
					node->list_items.push_back(item);
				}
			}
		}
		// mul int, string
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_STRING)
		{
			node->type = TYPE_STRING;

			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			for (int i = 0; i < node->left->int_value; i++)
			{
				*string = *string + *node->right->string_value;
			}

			node->string_value = string;
		}
		// mul int, bool
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->int_value * node->right->bool_value;
		}

		//---- FLOAT ----//

		// mul float, int
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value * node->right->int_value;
		}
		// mul float, float
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value * node->right->float_value;
		}
		// mul float, bool
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value * node->right->bool_value;
		}

		//---- LIST ----//

		// mul list, int
		else if (node->left->type == TYPE_LIST && node->right->type == TYPE_INT)
		{
			node->type = TYPE_LIST;
			for (int i = 0; i < node->right->int_value; i++)
			{
				for (auto& item : node->left->list_items)
				{
					node->list_items.push_back(item);
				}
			}
		}
		//---- STRING ----//

		// mul string, int
		else if (node->left->type == TYPE_STRING && node->right->type == TYPE_INT)
		{
			node->type = TYPE_STRING;

			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			for (int i = 0; i < node->right->int_value; i++)
			{
				*string = *string + *node->left->string_value;
			}

			node->string_value = string;
		}

		//---- BOOL ----//

		// mul bool, int
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->bool_value * node->right->int_value;
		}
		// mul bool, float
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->bool_value * node->right->float_value;
		}
		// mul bool, bool
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->bool_value * node->right->bool_value;
		}

		else
		{
		std::cout << "\n" << log_error(node, not_implemented_error(node));
		node->type = TYPE_ERROR;
		}
	}

	// ########### DIV ########### //

	void eval_div(std::shared_ptr<Node>& node)
	{
		eval(node->left);
		eval(node->right);

		//---- ERROR ----//

		if (node->left->type == TYPE_ERROR || node->right->type == TYPE_ERROR)
		{
			std::cout << "\n" << log_error(node, "Malformed '" + type_repr(node->type) + "' statement.");
			node->type = TYPE_ERROR;
			return;
		}

		//---- INT ----//

		// div int, int
		if (node->left->type == TYPE_INT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = (float)node->left->int_value / (float)node->right->int_value;
		}
		// div int, float
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = (float)node->left->int_value / node->right->float_value;
		}
		// div int, bool
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->float_value = (float)node->left->int_value / node->right->bool_value;
		}

		//---- FLOAT ----//

		// div float, int
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value / node->right->int_value;
		}
		// div float, float
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value / node->right->float_value;
		}
		// div float, bool
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->float_value / node->right->bool_value;
		}

		//---- LIST ----//

		//---- STRING ----//

		//---- BOOL ----//

		// div bool, int
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->bool_value / node->right->int_value;
		}
		// div bool, float
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = node->left->bool_value / node->right->float_value;
		}
		// div bool, bool
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = node->left->bool_value / node->right->bool_value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### NEG ########### //

	void eval_neg(std::shared_ptr<Node>& node)
	{
		eval(node->right);

		//---- ERROR ----//

		if (node->right->type == TYPE_ERROR)
		{
			std::cout << "\n" << log_error(node, "Malformed '" + type_repr(node->type) + "' statement.");
			node->type = TYPE_ERROR;
			return;
		}

		//---- INT ----//

		// neg int
		if (node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->int_value = -node->right->int_value;
		}
		// neg float
		else if (node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->float_value = -node->right->float_value;
		}
		// neg list
		else if (node->right->type == TYPE_LIST)
		{
			node->type = TYPE_LIST;

			for (int i = node->right->list_items.size() - 1; i > 0; i--)
			{
				node->list_items.push_back(node->right->list_items[i]);
			}
		}
		// neg string
		else if (node->right->type == TYPE_STRING)
		{
			node->type = TYPE_STRING;
			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			for (int i = (*node->right->string_value).length() - 1; i > 0; i--)
			{
				*string = *string + (*node->right->string_value)[i];
			}

			node->string_value = string;
		}
		// neg bool
		else if (node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = -(int)(node->right->bool_value);
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### POS ########### //

	void eval_pos(std::shared_ptr<Node>& node)
	{
		eval(node->right);

		//---- ERROR ----//

		if (node->right->type == TYPE_ERROR)
		{
			std::cout << "\n" << log_error(node, "Malformed '" + type_repr(node->type) + "' statement.");
			node->type = TYPE_ERROR;
			return;
		}

		//---- INT ----//

		// pos int
		if (node->right->type == TYPE_INT)
		{
			return;
		}
		// pos float
		else if (node->right->type == TYPE_FLOAT)
		{
			return;
		}
		// pos list
		else if (node->right->type == TYPE_LIST)
		{
			return;
		}
		// pos string
		else if (node->right->type == TYPE_STRING)
		{
			return;
		}
		// pos bool
		else if (node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->int_value = node->bool_value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### ASSIGNMENT ########### //

	std::string type_repr_builtin(Type type)
	{
		switch (type)
		{
			case TYPE_INT:
				return "int";
			case TYPE_FLOAT:
				return "float";
			case TYPE_BOOL:
				return "bool";
			case TYPE_STRING:
				return "string";
			case TYPE_SCOPE:
				return "scope";
			default:
				return "undefined";
		}
	}

	void eval_assignment(std::shared_ptr<Node>& node)
	{
		eval(node->right);

		if (node->right->type == TYPE_ERROR)
		{
			node->type = TYPE_ERROR;
			return;
		}

		node->right->type_type = type_repr_builtin(node->right->type);

		if (node->left->type != TYPE_ID && node->left->type != TYPE_COLON && node->left->type != TYPE_DOUBLE_COLON)
		{
			std::cout << "\n" << log_error(node, "Cannot assign to non-identifier.");
			node->type = TYPE_ERROR;
			return;
		}

		if (node->left->type == TYPE_ID)
		{
			std::string var_name = node->left->get_id_value();

			eval_id(node->left, false);

			if (node->left->type == TYPE_EMPTY)
			{
				// create a copy of the right node and assign it
				std::shared_ptr<Node> right = std::make_shared<Node>(*node->right);
				current_scope->scope_data[var_name] = { node->right->type_type, right };
				return;
			}
			else
			{
				std::string left_type = type_repr_builtin(node->left->type);
				if (left_type != node->right->type_type)
				{
					std::cout << "\n" << log_error(node, "Cannot assign value of type '" + node->right->type_type + "' to variable of type '" + left_type + "'.");
					node->type = TYPE_ERROR;
					return;
				}
				else
				{
					*node->left = *node->right;
				}

				return;
			}
		}
		else if (node->left->type == TYPE_COLON)
		{
			eval(node->left);
			if (node->left->type == TYPE_ERROR)
			{
				node->type = TYPE_ERROR;
				return;
			}
			if (node->left->type_type == "any")
			{
				current_scope->scope_data[node->left->get_id_value()] = { node->right->type_type, node->right };
			}
			else
			{
				current_scope->scope_data[node->left->get_id_value()] = { node->left->type_type, node->right };
			}
		}
		else if (node->left->type == TYPE_DOUBLE_COLON)
		{
			eval(node->left);

			if (node->left->type == TYPE_ERROR)
			{
				node->type = TYPE_ERROR;
				return;
			}

			if (node->left->type_type == "any")
				current_scope->scope_data[node->left->get_id_value()] = { node->right->type_type, node->right };

			else
			{
				if (node->left->type_type != node->right->type_type)
				{
					std::cout << "\n" << log_error(node, "Cannot assign value of type '" + node->right->type_type + "' to variable of type '" + node->left->type_type + "'.");
					node->type = TYPE_ERROR;
					return;
				}
				else
				{
					*node->left = *node->right;
				}
			}
			return;
		}
		else
		{
			node->type = TYPE_ERROR;
			return;
		}
	}

	// ########### BLOCK ########### //

	void eval_block(std::shared_ptr<Node>& node)
	{
		std::shared_ptr<Node> scope;

		if (node->block_name.empty())
		{
			scope = new_scope();
		}
		else
		{
			scope = new_scope(node->block_name);
		}

		enter_scope(scope);

		for (auto& expr : node->body)
		{
			eval(expr);
		}

		exit_scope();
	}

	// ########### DOUBLE_COLON ########### //

	void eval_scope_accessor(std::shared_ptr<Node>& node)
	{
		std::shared_ptr<Node> scope;

		if (node->left->type == TYPE_ID)
		{
			scope = get_scope(node->left->get_id_value());
		}
		else if (node->left->type == TYPE_DOUBLE_COLON)
		{
			eval_scope_accessor(node->left);
			scope = node->left;
		}
		else if (node->left->type == TYPE_CALL)
		{
			eval_call(node->left);
			scope = node->left;
		}

		if (!scope)
		{
			std::cout << "\n" << log_error(node, "Scope '" + node->left->get_id_value() + "' is not defined in current or outer scopes.");
			node->type = TYPE_ERROR;
			return;
		}

		if (scope->type != TYPE_ID && scope->type != TYPE_SCOPE)
		{
			std::cout << "\n" << log_error(node, "Scope must be an identifier or scope.");
			node->type = TYPE_ERROR;
			return;
		}

		std::shared_ptr<Node>& var = get_var_in_scope(node->right, scope);

		if (var)
		{
			node = var;
			return;
		}

		if (!var)
		{
			std::cout << "\n" << log_error(node, "'" + node->right->get_id_value() + "' is not defined in scope '" + scope->scope_name + "'.");
			node->type = TYPE_ERROR;
			return;
		}
	}

	// ########### SCOPE ########### //

	std::shared_ptr<Node> new_scope(std::string name = "")
	{
		__scopes_num++;
		std::shared_ptr<Node> scope = std::make_shared<Node>(TYPE_SCOPE);
		if (name.length() == 0)
		{
			scope->scope_name = std::to_string(__scopes_num);
		}
		else
		{
			scope->scope_is_named = true;
			scope->scope_name = name;
		}

		scope->scope_parent = current_scope;
		current_scope->scope_data[scope->scope_name] = { "scope", scope };
		return scope;
	}

	void enter_scope(std::shared_ptr<Node>& scope)
	{
		current_scope = scope;
	}

	void clear_scope(std::shared_ptr<Node>& scope)
	{
		scope->scope_data.clear();
	}

	void delete_scope(std::shared_ptr<Node>& scope)
	{
		scope->scope_parent->scope_data.erase(scope->scope_name);
	}

	void exit_scope()
	{
		if (!current_scope->scope_is_named)
		{
			clear_scope(current_scope);
			delete_scope(current_scope);
		}
		current_scope = current_scope->scope_parent;
	}

	std::shared_ptr<Node>& get_scope(std::string scope_name)
	{
		std::shared_ptr<Node> scope = current_scope;

		std::shared_ptr<Node>& searched_scope = scope->scope_data[scope_name].second;

		while (!searched_scope)
		{
			scope = scope->scope_parent;

			if (scope == nullptr)
			{
				return searched_scope;
			}

			searched_scope = scope->scope_data[scope_name].second;
		}

		return searched_scope;
	}

	std::shared_ptr<Node>& get_scope_in_scope(std::shared_ptr<Node> id, std::shared_ptr<Node>& scope)
	{
		std::shared_ptr<Node>& searched_scope = scope->scope_data[id->get_id_value()].second;
		return searched_scope;
	}

	// ########### ID ########### //

	std::shared_ptr<Variable> get_var(std::shared_ptr<Node> node)
	{
		std::shared_ptr<Variable> var_info = std::make_shared<Variable>();

		var_info->id = node->get_id_value();

		std::shared_ptr<Node> scope = current_scope;

		std::shared_ptr<Node>& var = scope->scope_data[node->get_id_value()].second;

		while (!var)
		{
			scope = scope->scope_parent;

			if (scope == nullptr)
			{
				var_info->not_found = true;
				return var_info;
			}

			var = scope->scope_data[node->get_id_value()].second;
		}

		var_info->id = node->get_id_value();
		var_info->scope = scope;
		return var_info;
	}

	std::shared_ptr<Node>& get_var_in_scope(std::shared_ptr<Node> node, std::shared_ptr<Node>& scope)
	{
		std::shared_ptr<Node>& var = scope->scope_data[node->get_id_value()].second;
		return var;
	}

	void eval_id(std::shared_ptr<Node>& node, bool standalone = true)
	{
		std::shared_ptr<Variable> var_info = get_var(node);

		// if called on its own, it errors if var doesn't exist. In cases such as assignment, standalone should be
		// false in order to return an EMPTY if it doesn't exist.

		if (standalone)
		{
			if (var_info->not_found)
			{
				std::cout << "\n" << log_error(node, "Variable '" + node->get_id_value() + "' is not defined in current or outer scopes.");
				node->type = TYPE_ERROR;
			}
			else
			{
				node = var_info->scope->scope_data[var_info->id].second;
			}
		}

		else
		{
			if (var_info->not_found)
			{
				node->type = TYPE_EMPTY;
			}
			else
			{
				node = var_info->scope->scope_data[var_info->id].second;
			}
		}
	}

	// ########### TYPE ASSIGNMENT ########### //

	bool get_type(std::string type)
	{
		std::shared_ptr<Node> scope = current_scope;

		while (!std::count(scope->scope_types.begin(), scope->scope_types.end(), type))
		{
			scope = scope->scope_parent;
			if (scope == nullptr)
			{
				return false;
			}
		}

		return true;
	}

	void eval_type_assignment(std::shared_ptr<Node>& node)
	{
		if (node->left->type != TYPE_ID && node->right->type != TYPE_ID)
		{
			std::cout << "\n" << log_error(node, "Cannot assign type to non-identifier.");
			node->type = TYPE_ERROR;
		}

		std::string type = node->right->get_id_value();

		bool type_exists = get_type(type);

		if (type_exists)
		{
			node->left->type_type = type;
			node = node->left;
			return;
		}
		else
		{
			std::cout << "\n" << log_error(node, "Type '" + type + "' is undefined.");
			node->type = TYPE_ERROR;
			return;
		}
	}

	void eval_call(std::shared_ptr<Node>& node)
	{
		if (*node->call_name == "print")
		{
			for (auto& arg : node->args)
			{
				eval(arg);
				print_node(arg);
			}

			return;
		}

		if (*node->call_name == "type_of")
		{
			if (node->args.size() != 1)
			{
				node->type == TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Built-in function 'type_of' only accepts one argument.");
				return;
			}

			auto& arg = node->args[0];

			eval(arg);
			node->type = TYPE_TYPE;
			node->type_type = arg->type_type;

			return;
		}

		if (*node->call_name == "str")
		{
			if (node->args.size() != 1)
			{
				node->type == TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Built-in function 'str' only accepts one argument.");
				return;
			}

			auto& arg = node->args[0];

			eval(arg);

			if (arg->type == TYPE_INT)
			{
				node->string_value = std::make_shared<std::string>(std::to_string(arg->int_value));
				node->type = TYPE_STRING;
			}
			else if (arg->type == TYPE_FLOAT)
			{
				node->string_value = std::make_shared<std::string>(std::to_string(arg->float_value));
				node->type = TYPE_STRING;
			}
			else if (arg->type == TYPE_BOOL)
			{
				node->string_value = std::make_shared<std::string>(std::to_string(arg->bool_value));
				node->type = TYPE_STRING;
			}
			else
			{
				node = arg;
			}

			return;
		}

		if (*node->call_name == "import")
		{
			if (node->args.size() != 1)
			{
				node->type = TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Built-in function 'import' only accepts one argument.");
				return;
			}

			auto& arg = node->args[0];

			if (arg->type != TYPE_STRING)
			{
				node->type = TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Built-in function 'import' expects a string argument.");
				return;
			}

			Lexer lexer(*arg->string_value);
			if (lexer.get_source().size() == 0)
			{
				node->type = TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Cannot import empty file.");
				return;
			}

			lexer.tokenize();

			Parser parser(lexer);
			parser.parse();

			Eval eval;

			eval.init();

			if (parser.expressions.size() == 0)
			{
				std::cin.get();
				return;
			}

			for (auto expr : parser.expressions)
			{
				eval.eval(expr);
			}

			node = eval.global_scope;
			return;
		}
	}
};