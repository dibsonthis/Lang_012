#pragma once
#include "Node.hpp"

void print_node(std::shared_ptr<Node> node)
{
	if (node == nullptr)
	{
		return;
	}
	else if (node->type == TYPE_INT)
	{
		std::cout << node->get_int_value();
	}
	else if (node->type == TYPE_FLOAT)
	{
		printf("%f", node->get_float_value());
	}
	else if (node->type == TYPE_STRING)
	{
		std::cout << node->get_string_value();
	}
	else if (node->type == TYPE_BOOL)
	{
		std::cout << node->bool_value;
	}
	else if (node->type == TYPE_ID)
	{
		std::cout << node->get_id_value();
	}
	else if (node->type == TYPE_DOT)
	{
		std::cout << "( ";
		print_node(node->left);
		std::cout << " . ";
		print_node(node->right);
		std::cout << " )";
	}
	else if (node->type == TYPE_COMMA)
	{
		std::cout << "( ";
		print_node(node->left);
		std::cout << " , ";
		print_node(node->right);
		std::cout << " )";
	}
	else if (node->type == TYPE_COLON)
	{
		std::cout << "( ";
		print_node(node->left);
		std::cout << " : ";
		print_node(node->right);
		std::cout << " )";
	}
	else if (node->type == TYPE_DOUBLE_COLON)
	{
		std::cout << "( ";
		print_node(node->left);
		std::cout << " :: ";
		print_node(node->right);
		std::cout << " )";
	}
	else if (node->type == TYPE_CALL)
	{
		std::cout << type_repr(node->type) << "(name: " << *node->call_name << ", args: [ ";
		for (int i = 0; i < node->args.size(); i++)
		{
			print_node(node->args[i]);

			if (i != node->args.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_BLOCK)
	{
		std::cout << type_repr(node->type) << "(statements: [ ";

		for (int i = 0; i < node->body.size(); i++)
		{
			print_node(node->body[i]);

			if (i != node->body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_IF)
	{
		std::cout << type_repr(node->type) << "(expression: ";
		print_node(node->if_else_expr);
		std::cout << ", body: [ ";

		for (int i = 0; i < node->if_else_body->body.size(); i++)
		{
			print_node(node->if_else_body->body[i]);

			if (i != node->if_else_body->body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_ELSE)
	{
		std::cout << type_repr(node->type) << "(body: [ ";

		for (int i = 0; i < node->if_else_body->body.size(); i++)
		{
			print_node(node->if_else_body->body[i]);

			if (i != node->if_else_body->body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_IF_ELSE_STATEMENT)
	{
		std::cout << type_repr(node->type) << "( ";
		for (int i = 0; i < node->if_else_statements.size(); i++)
		{
			print_node(node->if_else_statements[i]);
			if (i != node->if_else_statements.size() - 1)
			{
				std::cout << ", ";
			}

		}
		std::cout << " )";
	}
	else if (node->type == TYPE_TYPE_DEF)
	{
		std::cout << type_repr(node->type) << "(type name: " << *node->type_name;
		std::cout << ", body: [ ";

		for (int i = 0; i < node->type_body.size(); i++)
		{
			print_node(node->type_body[i]);

			if (i != node->type_body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_FUNC_DEF)
	{
		std::cout << type_repr(node->type) << "(func name: " << *node->func_name;

		std::cout << ", params: [ ";

		for (int i = 0; i < node->func_params.size(); i++)
		{
			print_node(node->func_params[i]);

			if (i != node->func_params.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ], return_type: ";

		if (node->return_type == nullptr)
			std::cout << "NONE";
		else
			print_node(node->return_type);

		std::cout << " , body: [ ";

		for (int i = 0; i < node->func_body.size(); i++)
		{
			print_node(node->func_body[i]);

			if (i != node->func_body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_RETURN)
	{
		std::cout << type_repr(node->type) << "( ";
		print_node(node->return_statement);
		std::cout << " )";
	}
	else if (node->type == TYPE_LIST)
	{
		std::cout << type_repr(node->type) << "[ ";
		for (int i = 0; i < node->list_items.size(); i++)
		{
			print_node(node->list_items[i]);

			if (i != node->list_items.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ]";
	}
	else if (node->type == TYPE_RANGE)
	{
		std::cout << type_repr(node->type) << "[ ";
		for (int i = 0; i < node->range_items.size(); i++)
		{
			print_node(node->range_items[i]);

			if (i != node->range_items.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ]";
	}
	else if (node->is_op)
	{
		std::cout << "( ";
		if (node->left != nullptr)
		{
			print_node(node->left);
			std::cout << " ";
		}
		std::cout << type_repr(node->type);
		if (node->right != nullptr)
		{
			std::cout << " ";
			print_node(node->right);
		}
		std::cout << " )";
	}
	else if (node->is_post_op)
	{
		std::cout << "( ";
		if (node->right != nullptr)
		{
			print_node(node->right);
			std::cout << " ";
		}
		std::cout << type_repr(node->type);
		std::cout << " )";
	}
	else if (node->type == TYPE_SCOPE)
	{
		std::cout << type_repr(node->type) << "(" << node->scope_parent->scope_name << "::" << node->scope_name << ")";
	}
	else if (node->type == TYPE_TYPE)
	{
		std::cout << node->type_type;
	}
}