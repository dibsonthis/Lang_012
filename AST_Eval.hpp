#pragma once

#include "AST_Parser.hpp"
#include "AST_Utils.hpp"
#include "Type.hpp"

std::string not_implemented_error(std::shared_ptr<AST_Node>& op)
{
	if (op->left == nullptr)
	{
		return "Cannot perform '" + type_repr(op->type) + "' on '" + type_repr(op->right->type) + "'.";
	}

	return "Cannot perform '" + type_repr(op->type) + "' on '" + type_repr(op->left->type) + "' and '" + type_repr(op->right->type) + "'.";
}

class AST_Eval
{
public:

	std::string file_name = "stdin";
	std::vector<std::string> errors;

	int __scopes_num = 0;

	AST_Eval() = default;

	AST_Eval(AST_Parser parser) : file_name(parser.file_name) {}

	std::shared_ptr<AST_Node> global_scope = std::make_shared<AST_Node>(TYPE_SCOPE);
	std::shared_ptr<AST_Node>& current_scope = global_scope;

	std::string log_error(std::shared_ptr<AST_Node>& error_node, std::string message)
	{
		std::string error_message = "[Eval] Evaluation Error in '" + file_name + "' @ (" + std::to_string(error_node->line) + ", " + std::to_string(error_node->column) + "): " + message;
		errors.push_back(error_message);
		return error_message;
	}

	void init()
	{
		global_scope->SCOPE.name = "global";
		global_scope->SCOPE.types =
		{
			"int", "bool", "float", "string", "scope", "any"
		};
	}

	std::shared_ptr<AST_Node> infer_type(std::shared_ptr<AST_Node>& node)
	{
		std::shared_ptr<AST_Node> type = std::make_shared<AST_Node>(TYPE_TYPE);

		switch (node->type)
		{
		case TYPE_INT:
			type->TYPE.name = "int";
			return type;
		case TYPE_FLOAT:
			type->TYPE.name = "float";
			return type;
		case TYPE_BOOL:
			type->TYPE.name = "bool";
			return type;
		case TYPE_STRING:
			type->TYPE.name = "string";
			return type;
		case TYPE_LIST:
			type->TYPE.name = "list";
			return type;
		case TYPE_TYPE:
			type->TYPE.name = "type";
			return type;
		case TYPE_SCOPE:
			type->TYPE.name = "scope";
			return type;
		case TYPE_VAR:
			type = infer_type(node->VAR.value);
			return type;
		default:
			type->TYPE.built_in = false;
			return type;
		}
	}

	int implicit_cast(std::shared_ptr<AST_Node>& value, std::string type)
	{
		int ok = 0;
		int data_loss = 1;
		int error = 2;

		if (value->type == TYPE_INT && type == "float")
		{
			value->type = TYPE_FLOAT;
			value->FLOAT.value = (float)value->INT.value;
			return ok;
		}
		if (value->type == TYPE_FLOAT && type == "int")
		{
			value->type = TYPE_INT;
			value->INT.value = (int)value->FLOAT.value;
			return data_loss;
		}
		if (value->type == TYPE_BOOL && type == "int")
		{
			value->type = TYPE_INT;
			value->INT.value = (int)value->BOOL.value;
			return ok;
		}
		if (value->type == TYPE_INT && type == "bool")
		{
			value->type = TYPE_BOOL;
			value->BOOL.value = (bool)value->INT.value;
			return data_loss;
		}
		if (value->type == TYPE_FLOAT && type == "bool")
		{
			value->type = TYPE_BOOL;
			value->BOOL.value = (bool)value->FLOAT.value;
			return data_loss;
		}
		if (value->type == TYPE_BOOL && type == "float")
		{
			value->type = TYPE_FLOAT;
			value->FLOAT.value = (float)value->BOOL.value;
			return ok;
		}

		return error;
	}

	std::shared_ptr<AST_Node> create_var(std::string name, std::shared_ptr<AST_Node> value, std::shared_ptr<AST_Node> type = nullptr)
	{
		auto var = std::make_shared<AST_Node>(TYPE_VAR);
		var->VAR.name = name;
		var->VAR.value = value;

		if (type)
		{
			var->VAR.type = type;
		}
		else
		{
			var->VAR.type = infer_type(var->VAR.value);
		}

		return var;
	}

	std::shared_ptr<AST_Node> get_data(std::string name)
	{
		auto scope = current_scope;

		auto var = get_data_from_scope(name, scope);

		while (!var)
		{
			scope = scope->SCOPE.parent;

			if (scope == nullptr)
			{
				return var;
			}

			var = get_data_from_scope(name, scope);
		}

		return var;
	}

	std::shared_ptr<AST_Node> get_data_from_scope(std::string name, std::shared_ptr<AST_Node> scope)
	{
		for (auto& data : scope->SCOPE.data)
		{
			if (data->VAR.name == name)
			{
				return data;
			}
		}

		return nullptr;
	}

	void eval(std::shared_ptr<AST_Node>& node)
	{
		if (!node)
		{
			node = std::make_shared<AST_Node>(TYPE_ERROR);
			return;
		}

		switch (node->type)
		{
		case TYPE_INT:
			return;
		case TYPE_FLOAT:
			return;
		case TYPE_BOOL:
			return;
		case TYPE_STRING:
			return;
		case TYPE_LIST:
			return;
		case TYPE_TYPE:
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
			//eval_type_assignment(node);
			return;
		case TYPE_CALL:
			eval_call(node);
			return;
		}
	}

	// ########### PLUS ########### //

	void eval_plus(std::shared_ptr<AST_Node>& node)
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
			node->INT.value = node->left->INT.value + node->right->INT.value;
		}
		// plus int, float
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->INT.value + node->right->FLOAT.value;
		}
		// plus int, bool
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->INT.value + node->right->BOOL.value;
		}

		//---- FLOAT ----//

		// plus float, int
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value + node->right->INT.value;
		}
		// plus float, float
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value + node->right->FLOAT.value;
		}
		// plus float, bool
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value + node->right->BOOL.value;
		}

		//---- LIST ----//

		// plus list, list
		else if (node->left->type == TYPE_LIST && node->right->type == TYPE_LIST)
		{
			node->type = TYPE_LIST;
			for (auto& item : node->left->LIST.items)
			{
				node->LIST.items.push_back(item);
			}
			for (auto& item : node->right->LIST.items)
			{
				node->LIST.items.push_back(item);
			}
		}

		//---- STRING ----//

		// plus string, string
		else if (node->left->type == TYPE_STRING && node->right->type == TYPE_STRING)
		{
			node->type = TYPE_STRING;
			std::shared_ptr<std::string> string = std::make_shared<std::string>();
			*string = node->left->STRING.value + node->right->STRING.value;
			node->STRING.value = *string;
		}

		//---- BOOL ----//

		// plus bool, int
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->BOOL.value + node->right->INT.value;
		}
		// plus bool, float
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->BOOL.value + node->right->FLOAT.value;
		}
		// plus bool, bool
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->BOOL.value + node->right->BOOL.value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### MINUS ########### //

	void eval_minus(std::shared_ptr<AST_Node>& node)
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
			node->INT.value = node->left->INT.value - node->right->INT.value;
		}
		// minus int, float
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->INT.value - node->right->FLOAT.value;
		}
		// minus int, string
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_STRING)
		{
			node->type = TYPE_STRING;

			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			int str_len = (node->right->STRING.value).size();

			if (node->left->INT.value > str_len)
			{
				node->type = TYPE_ERROR;
				return;
			}

			for (int i = node->left->INT.value; i < str_len; i++)
			{
				*string = *string + (node->right->STRING.value)[i];
			}

			node->STRING.value = *string;
		}
		// minus int, bool
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->INT.value - node->right->BOOL.value;
		}

		//---- FLOAT ----//

		// minus float, int
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value - node->right->INT.value;
		}
		// minus float, float
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value - node->right->FLOAT.value;
		}
		// minus float, bool
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value - node->right->BOOL.value;
		}

		//---- LIST ----//

		//---- STRING ----//

		// minus string, int
		else if (node->left->type == TYPE_STRING && node->right->type == TYPE_INT)
		{
			node->type = TYPE_STRING;

			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			int str_len = (node->left->STRING.value).size() - node->right->INT.value;

			if (str_len < 0)
			{
				node->type = TYPE_ERROR;
				return;
			}

			for (int i = 0; i < str_len; i++)
			{
				*string = *string + (node->left->STRING.value)[i];
			}

			node->STRING.value = *string;
		}

		//---- BOOL ----//

		// minus bool, int
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->BOOL.value - node->right->INT.value;
		}
		// minus bool, float
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->BOOL.value - node->right->FLOAT.value;
		}
		// minus bool, bool
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->BOOL.value - node->right->BOOL.value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}

	}

	void eval_mul(std::shared_ptr<AST_Node>& node)
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
			node->INT.value = node->left->INT.value * node->right->INT.value;
		}
		// mul int, float
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->INT.value * node->right->FLOAT.value;
		}
		// mul int, list
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_LIST)
		{
			node->type = TYPE_LIST;
			for (int i = 0; i < node->left->INT.value; i++)
			{
				for (auto& item : node->right->LIST.items)
				{
					node->LIST.items.push_back(item);
				}
			}
		}
		// mul int, string
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_STRING)
		{
			node->type = TYPE_STRING;

			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			for (int i = 0; i < node->left->INT.value; i++)
			{
				*string = *string + node->right->STRING.value;
			}

			node->STRING.value = *string;
		}
		// mul int, bool
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->INT.value * node->right->BOOL.value;
		}

		//---- FLOAT ----//

		// mul float, int
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value * node->right->INT.value;
		}
		// mul float, float
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value * node->right->FLOAT.value;
		}
		// mul float, bool
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value * node->right->BOOL.value;
		}

		//---- LIST ----//

		// mul list, int
		else if (node->left->type == TYPE_LIST && node->right->type == TYPE_INT)
		{
			node->type = TYPE_LIST;
			for (int i = 0; i < node->right->INT.value; i++)
			{
				for (auto& item : node->left->LIST.items)
				{
					node->LIST.items.push_back(item);
				}
			}
		}
		//---- STRING ----//

		// mul string, int
		else if (node->left->type == TYPE_STRING && node->right->type == TYPE_INT)
		{
			node->type = TYPE_STRING;

			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			for (int i = 0; i < node->right->INT.value; i++)
			{
				*string = *string + node->left->STRING.value;
			}

			node->STRING.value = *string;
		}

		//---- BOOL ----//

		// mul bool, int
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->BOOL.value * node->right->INT.value;
		}
		// mul bool, float
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->BOOL.value * node->right->FLOAT.value;
		}
		// mul bool, bool
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->BOOL.value * node->right->BOOL.value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### DIV ########### //

	void eval_div(std::shared_ptr<AST_Node>& node)
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
			node->FLOAT.value = (float)node->left->INT.value / (float)node->right->INT.value;
		}
		// div int, float
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = (float)node->left->INT.value / node->right->FLOAT.value;
		}
		// div int, bool
		else if (node->left->type == TYPE_INT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = (float)node->left->INT.value / node->right->BOOL.value;
		}

		//---- FLOAT ----//

		// div float, int
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_INT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value / node->right->INT.value;
		}
		// div float, float
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value / node->right->FLOAT.value;
		}
		// div float, bool
		else if (node->left->type == TYPE_FLOAT && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->FLOAT.value / node->right->BOOL.value;
		}

		//---- LIST ----//

		//---- STRING ----//

		//---- BOOL ----//

		// div bool, int
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_INT)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->BOOL.value / node->right->INT.value;
		}
		// div bool, float
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = node->left->BOOL.value / node->right->FLOAT.value;
		}
		// div bool, bool
		else if (node->left->type == TYPE_BOOL && node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->INT.value = node->left->BOOL.value / node->right->BOOL.value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### NEG ########### //

	void eval_neg(std::shared_ptr<AST_Node>& node)
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
			node->INT.value = -node->right->INT.value;
		}
		// neg float
		else if (node->right->type == TYPE_FLOAT)
		{
			node->type = TYPE_FLOAT;
			node->FLOAT.value = -node->right->FLOAT.value;
		}
		// neg list
		else if (node->right->type == TYPE_LIST)
		{
			node->type = TYPE_LIST;

			for (int i = node->right->LIST.items.size() - 1; i > 0; i--)
			{
				node->LIST.items.push_back(node->right->LIST.items[i]);
			}
		}
		// neg string
		else if (node->right->type == TYPE_STRING)
		{
			node->type = TYPE_STRING;
			std::shared_ptr<std::string> string = std::make_shared<std::string>();

			for (int i = (node->right->STRING.value).length() - 1; i > 0; i--)
			{
				*string = *string + (node->right->STRING.value)[i];
			}

			node->STRING.value = *string;
		}
		// neg bool
		else if (node->right->type == TYPE_BOOL)
		{
			node->type = TYPE_INT;
			node->INT.value = -(int)(node->right->BOOL.value);
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### POS ########### //

	void eval_pos(std::shared_ptr<AST_Node>& node)
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
			node->INT.value = node->BOOL.value;
		}

		else
		{
			std::cout << "\n" << log_error(node, not_implemented_error(node));
			node->type = TYPE_ERROR;
		}
	}

	// ########### ASSIGNMENT ########### //

	void eval_assignment(std::shared_ptr<AST_Node>& node)
	{
		eval(node->right);

		if (node->right->type == TYPE_ERROR)
		{
			node->type = TYPE_ERROR;
			return;
		}

		if (node->left->type == TYPE_DOUBLE_COLON)
		{
			eval_scope_accessor(node->left);

			// do type_check
			auto right_type = infer_type(node->right);
			if (node->left->VAR.type->TYPE.name == right_type->TYPE.name)
			{
				node->left->VAR.value = node->right;
				node->left->VAR.type = right_type;
			}
			else
			{
				int impl_cast = implicit_cast(node->right, node->left->VAR.type->TYPE.name);

				if (impl_cast == 0)
				{
					node->left->VAR.value = node->right;
					node->left->VAR.type = infer_type(node->right);
				}
				else if (impl_cast == 1)
				{
					node->left->VAR.value = node->right;
					node->left->VAR.type = infer_type(node->right);
					std::cout << "\n" << "Warning: Potential data loss...";
				}
				else
				{
					std::cout << "\n" << log_error(node, "Cannot assign value of type '" + right_type->TYPE.name + "' to variable of type '" + node->left->VAR.type->TYPE.name + "'.");
					node->type = TYPE_ERROR;
					return;
				}
			}

		}

		else if (node->left->type == TYPE_ID)
		{
			std::shared_ptr<AST_Node> var = get_data(node->left->ID.value);
			if (var)
			{
				// do type_check
				auto right_type = infer_type(node->right);
				if (var->VAR.type->TYPE.name == right_type->TYPE.name)
				{
					var->VAR.value = node->right;
					var->VAR.type = right_type;
				}
				else
				{
					int impl_cast = implicit_cast(node->right, var->VAR.type->TYPE.name);

					if (impl_cast == 0)
					{
						var->VAR.value = node->right;
						var->VAR.type = infer_type(node->right);
					}
					else if (impl_cast == 1)
					{
						var->VAR.value = node->right;
						var->VAR.type = infer_type(node->right);
						std::cout << "\n" << "Warning: Potential data loss...";
					}
					else
					{
						std::cout << "\n" << log_error(node, "Cannot assign value of type '" + right_type->TYPE.name + "' to variable of type '" + var->VAR.type->TYPE.name + "'.");
						node->type = TYPE_ERROR;
						return;
					}
				}
			}
			else
			{
				var = std::make_shared<AST_Node>(TYPE_VAR);
				var->VAR.name = node->left->ID.value;
				var->VAR.value = node->right;
				var->VAR.type = infer_type(node->right);
				current_scope->SCOPE.data.push_back(var);
			}
		}
	}

	// ########### BLOCK ########### //

	void eval_block(std::shared_ptr<AST_Node>& node)
	{
		std::shared_ptr<AST_Node> scope;

		if (node->BLOCK.name.empty())
		{
			scope = new_scope();
		}
		else
		{
			scope = new_scope(node->BLOCK.name);
		}

		enter_scope(scope);

		for (auto& expr : node->BLOCK.body)
		{
			eval(expr);
		}

		exit_scope();
	}

	// ########### DOUBLE_COLON ########### //

	void eval_scope_accessor(std::shared_ptr<AST_Node>& node)
	{
		std::shared_ptr<AST_Node> scope = nullptr;

		if (node->left->type == TYPE_ID)
		{
			auto data = get_data(node->left->ID.value);
			scope = data->VAR.value;
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
			std::cout << "\n" << log_error(node, "Scope '" + node->left->ID.value + "' is not defined in current or outer scopes.");
			node->type = TYPE_ERROR;
			return;
		}

		if (scope->type == TYPE_VAR)
		{
			scope = scope->VAR.value;
		}

		std::shared_ptr<AST_Node> var = get_data_from_scope(node->right->ID.value, scope);

		if (var)
		{
			node = var;
			return;
		}

		if (!var)
		{
			std::cout << "\n" << log_error(node, "'" + node->right->ID.value + "' is not defined in scope '" +
				scope->SCOPE.name + "'.");
			node->type = TYPE_ERROR;
			return;
		}
	}

	// ########### SCOPE ########### //

	std::shared_ptr<AST_Node> new_scope(std::string name = "")
	{
		__scopes_num++;
		std::shared_ptr<AST_Node> scope = std::make_shared<AST_Node>(TYPE_SCOPE);
		if (name.length() == 0)
		{
			scope->SCOPE.name = std::to_string(__scopes_num);
		}
		else
		{
			scope->SCOPE.is_named = true;
			scope->SCOPE.name = name;
		}

		scope->SCOPE.parent = current_scope;

		std::shared_ptr<AST_Node> scope_var = std::make_shared<AST_Node>(TYPE_VAR);
		scope_var->VAR.value = scope;
		scope_var->VAR.name = scope->SCOPE.name;
		std::shared_ptr<AST_Node> scope_var_type = std::make_shared<AST_Node>(TYPE_TYPE);
		scope_var_type->TYPE.name = "scope";
		scope_var->VAR.type = scope_var_type;

		current_scope->SCOPE.data.push_back(scope_var);
		return scope;
	}

	void enter_scope(std::shared_ptr<AST_Node>& scope)
	{
		current_scope = scope;
	}

	void clear_scope(std::shared_ptr<AST_Node>& scope)
	{
		scope->SCOPE.data.clear();
	}

	void delete_scope(std::shared_ptr<AST_Node>& scope)
	{
		auto& data = scope->SCOPE.parent->SCOPE.data;

		for (int i = 0; i < data.size(); i++)
		{
			if (data[i] == scope)
			{
				data.erase(data.begin() + i);
				return;
			}
		}
	}

	void exit_scope()
	{
		if (!current_scope->SCOPE.is_named)
		{
			clear_scope(current_scope);
			delete_scope(current_scope);
		}
		current_scope = current_scope->SCOPE.parent;
	}

	// ########### ID ########### //

	void eval_id(std::shared_ptr<AST_Node>& node, bool standalone = true)
	{
		std::shared_ptr<AST_Node> var = get_data(node->ID.value);

		if (standalone)
		{
			if (!var)
			{
				std::cout << "\n" << log_error(node, "Variable '" + node->ID.value + "' is not defined.");
				node->type = TYPE_ERROR;
				return;
			}
			else
			{
				node = var;
				return;
			}
		}
		else
		{
			if (!var)
			{
				node->type = TYPE_EMPTY;
				return;
			}
			else
			{
				node = var;
				return;
			}
		}

		return;
	}

	// ########### TYPE ASSIGNMENT ########### //


	void eval_call(std::shared_ptr<AST_Node>& node)
	{
		if (node->CALL.name == "print")
		{
			for (auto& arg : node->CALL.args)
			{
				eval(arg);
				print_ast_node(arg);
			}

			node->type = TYPE_EMPTY;
			return;
		}

		if (node->CALL.name == "type_of")
		{
			if (node->CALL.args.size() != 1)
			{
				node->type = TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Built-in function 'type_of' only accepts one argument.");
				return;
			}

			auto& arg = node->CALL.args[0];

			eval(arg);

			node = infer_type(arg);

			return;
		}

		if (node->CALL.name == "str")
		{
			if (node->CALL.args.size() != 1)
			{
				node->type = TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Built-in function 'str' only accepts one argument.");
				return;
			}

			auto& arg = node->CALL.args[0];

			if (arg->type != TYPE_STRING)
			{
				arg->type = TYPE_ERROR;
				std::cout << "\n" + log_error(arg, "Built-in function 'import' expects a string argument.");
				return;
			}

			call_str(arg);
			node = arg;
		}

		if (node->CALL.name == "ref")
		{
			if (node->CALL.args.size() != 1)
			{
				node->type = TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Built-in function 'ref' only accepts one argument.");
				return;
			}

			auto& arg = node->CALL.args[0];

			eval(arg);

			node->type = TYPE_REF;
			node->REF.ref = arg;
			return;
		}

		if (node->CALL.name == "import")
		{
			if (node->CALL.args.size() != 1)
			{
				node->type = TYPE_ERROR;
				std::cout << "\n" + log_error(node, "Built-in function 'import' only accepts one argument.");
				return;
			}

			auto& arg = node->CALL.args[0];

			call_import(arg);
			node = arg;
		}
	}

	void call_str(std::shared_ptr<AST_Node>& arg)
	{
		eval(arg);

		if (arg->type == TYPE_INT)
		{
			arg->STRING.value = std::to_string(arg->INT.value);
			arg->type = TYPE_STRING;
		}
		else if (arg->type == TYPE_FLOAT)
		{
			arg->STRING.value = std::to_string(arg->FLOAT.value);
			arg->type = TYPE_STRING;
		}
		else if (arg->type == TYPE_BOOL)
		{
			arg->STRING.value = std::to_string(arg->BOOL.value);
			arg->type = TYPE_STRING;
		}
		else if (arg->type == TYPE_VAR)
		{
			call_str(arg->VAR.value);
			arg = arg->VAR.value;
		}

		return;
	}

	void call_import(std::shared_ptr<AST_Node>& arg)
	{
		Lexer lexer(arg->STRING.value);

		if (lexer.get_source().size() == 0)
		{
			arg->type = TYPE_ERROR;
			std::cout << "\n" + log_error(arg, "Cannot import empty file.");
			return;
		}

		lexer.tokenize();

		AST_Parser parser(lexer);
		parser.parse();

		AST_Eval eval;

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

		*arg = *eval.global_scope;
		return;
	}
};