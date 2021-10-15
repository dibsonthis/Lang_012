#pragma once
#include "AST_Node.hpp"

void print_ast_node(std::shared_ptr<AST_Node> node)
{
	if (node == nullptr)
	{
		return;
	}
	else if (node->type == TYPE_INT)
	{
		std::cout << node->INT.value;
	}
	else if (node->type == TYPE_FLOAT)
	{
		printf("%f", node->FLOAT.value);
	}
	else if (node->type == TYPE_STRING)
	{
		std::cout << node->STRING.value;
	}
	else if (node->type == TYPE_BOOL)
	{
		std::cout << node->BOOL.value;
	}
	else if (node->type == TYPE_ID)
	{
		std::cout << node->ID.value;
	}
	else if (node->type == TYPE_VAR)
	{
		print_ast_node(node->VAR.value);
	}
	else if (node->type == TYPE_DOT)
	{
		std::cout << "( ";
		print_ast_node(node->left);
		std::cout << " . ";
		print_ast_node(node->right);
		std::cout << " )";
	}
	else if (node->type == TYPE_COMMA)
	{
		std::cout << "( ";
		print_ast_node(node->left);
		std::cout << " , ";
		print_ast_node(node->right);
		std::cout << " )";
	}
	else if (node->type == TYPE_COLON)
	{
		std::cout << "( ";
		print_ast_node(node->left);
		std::cout << " : ";
		print_ast_node(node->right);
		std::cout << " )";
	}
	else if (node->type == TYPE_DOUBLE_COLON)
	{
		std::cout << "( ";
		print_ast_node(node->left);
		std::cout << " :: ";
		print_ast_node(node->right);
		std::cout << " )";
	}
	else if (node->type == TYPE_CALL)
	{
		std::cout << type_repr(node->type) << "(name: " << node->CALL.name << ", args: [ ";
		for (int i = 0; i < node->CALL.args.size(); i++)
		{
			print_ast_node(node->CALL.args[i]);

			if (i != node->CALL.args.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_BLOCK)
	{
		std::cout << type_repr(node->type) << "(statements: [ ";

		for (int i = 0; i < node->BLOCK.body.size(); i++)
		{
			print_ast_node(node->BLOCK.body[i]);

			if (i != node->BLOCK.body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_IF)
	{
		std::cout << type_repr(node->type) << "(expression: ";
		print_ast_node(node->IF.expr);
		std::cout << ", body: [ ";

		for (int i = 0; i < node->IF.body->BLOCK.body.size(); i++)
		{
			print_ast_node(node->IF.body->BLOCK.body[i]);

			if (i != node->IF.body->BLOCK.body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_ELSE)
	{
		std::cout << type_repr(node->type) << "(body: [ ";

		for (int i = 0; i < node->IF.body->BLOCK.body.size(); i++)
		{
			print_ast_node(node->IF.body->BLOCK.body[i]);

			if (i != node->IF.body->BLOCK.body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_IF_ELSE_STATEMENT)
	{
		std::cout << type_repr(node->type) << "( ";
		for (int i = 0; i < node->IF_STATEMENT.statements.size(); i++)
		{
			print_ast_node(node->IF_STATEMENT.statements[i]);
			if (i != node->IF_STATEMENT.statements.size() - 1)
			{
				std::cout << ", ";
			}

		}
		std::cout << " )";
	}
	else if (node->type == TYPE_TYPE_DEF)
	{
		std::cout << type_repr(node->type) << "(type name: " << node->TYPE_DEF.name;
		std::cout << ", body: [ ";

		for (int i = 0; i < node->TYPE_DEF.body.size(); i++)
		{
			print_ast_node(node->TYPE_DEF.body[i]);

			if (i != node->TYPE_DEF.body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_FUNC_DEF)
	{
		std::cout << type_repr(node->type) << "(func name: " << node->FUNC_DEF.name;

		std::cout << ", params: [ ";

		for (int i = 0; i < node->FUNC_DEF.params.size(); i++)
		{
			print_ast_node(node->FUNC_DEF.params[i]);

			if (i != node->FUNC_DEF.params.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ], return_type: ";

		if (node->FUNC_DEF.return_type == nullptr)
			std::cout << "NONE";
		else
			print_ast_node(node->FUNC_DEF.return_type);

		std::cout << " , body: [ ";

		for (int i = 0; i < node->FUNC_DEF.body.size(); i++)
		{
			print_ast_node(node->FUNC_DEF.body[i]);

			if (i != node->FUNC_DEF.body.size() - 1)
			{
				std::cout << ", ";
			}
		}

		std::cout << " ])";
	}
	else if (node->type == TYPE_RETURN)
	{
		std::cout << type_repr(node->type) << "( ";
		print_ast_node(node->RETURN.value);
		std::cout << " )";
	}
	else if (node->type == TYPE_LIST)
	{
		std::cout << type_repr(node->type) << "[ ";
		for (int i = 0; i < node->LIST.items.size(); i++)
		{
			print_ast_node(node->LIST.items[i]);

			if (i != node->LIST.items.size() - 1)
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
			print_ast_node(node->left);
			std::cout << " ";
		}
		std::cout << type_repr(node->type);
		if (node->right != nullptr)
		{
			std::cout << " ";
			print_ast_node(node->right);
		}
		std::cout << " )";
	}
	else if (node->is_post_op)
	{
		std::cout << "( ";
		if (node->right != nullptr)
		{
			print_ast_node(node->right);
			std::cout << " ";
		}
		std::cout << type_repr(node->type);
		std::cout << " )";
	}
	else if (node->type == TYPE_SCOPE)
	{
		std::cout << type_repr(node->type) << "(" << node->SCOPE.parent->SCOPE.name << "::" << node->SCOPE.name << ")";
	}
	else if (node->type == TYPE_TYPE)
	{
		std::cout << node->TYPE.name;
	}
}