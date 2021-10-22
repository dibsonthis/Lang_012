#include "AST_Node.hpp"

std::shared_ptr<AST_Node> create_ref(std::shared_ptr<AST_Node> node)
{
	return node;
}

std::shared_ptr<AST_Node> create_copy(std::shared_ptr<AST_Node> node)
{
	std::shared_ptr<AST_Node> r = std::make_shared<AST_Node>(*node);
	return r;
}

std::shared_ptr<AST_Node> deep_copy(std::shared_ptr<AST_Node> node)
{
	if (node == nullptr)
		return nullptr;

	std::shared_ptr<AST_Node> node_copy = std::make_shared<AST_Node>(*node);

	node_copy->left = deep_copy(node->left);
	node_copy->right = deep_copy(node->right);

	node_copy->INT.value = node->INT.value;
	node_copy->FLOAT.value = node->FLOAT.value;
	node_copy->BOOL.value = node->BOOL.value;
	node_copy->STRING.value = node->STRING.value;
	node_copy->ID.value = node->ID.value;

	node_copy->CALL.name = node->CALL.name;
	node_copy->CALL.args.clear();
	for (auto arg : node->CALL.args)
	{
		node_copy->CALL.args.push_back(deep_copy(arg));
	}

	node_copy->FUNC_DEF.name = node->FUNC_DEF.name;
	node_copy->FUNC_DEF.params.clear();
	node_copy->FUNC_DEF.body.clear();
	for (auto param : node->FUNC_DEF.params)
	{
		node_copy->FUNC_DEF.params.push_back(deep_copy(param));
	}
	for (auto expr : node->FUNC_DEF.body)
	{
		node_copy->FUNC_DEF.body.push_back(deep_copy(expr));
	}

	node_copy->BLOCK.name = node->BLOCK.name;
	node_copy->BLOCK.body.clear();
	for (auto expr : node->BLOCK.body)
	{
		node_copy->BLOCK.body.push_back(deep_copy(expr));
	}

	node_copy->IF.body = deep_copy(node->IF.body);
	node_copy->IF.expr = deep_copy(node->IF.expr);

	node_copy->IF_STATEMENT.statements.clear();
	for (auto stmnt : node->IF_STATEMENT.statements)
	{
		node_copy->IF_STATEMENT.statements.push_back(deep_copy(stmnt));
	}

	node_copy->REF.ref = deep_copy(node->REF.ref);

	node_copy->TYPE.built_in = node->TYPE.built_in;
	node_copy->TYPE.name = node->TYPE.name;

	node_copy->VAR.name = node->VAR.name;
	node_copy->VAR.type = deep_copy(node->VAR.type);
	node_copy->VAR.value = deep_copy(node->VAR.value);

	node_copy->TYPE_DEF.name = node->TYPE_DEF.name;
	node_copy->TYPE_DEF.body.clear();
	for (auto expr : node->TYPE_DEF.body)
	{
		node_copy->TYPE_DEF.body.push_back(deep_copy(expr));
	}

	node_copy->RETURN.value = deep_copy(node->RETURN.value);

	node_copy->LIST.items.clear();
	for (auto item : node->LIST.items)
	{
		node_copy->LIST.items.push_back(deep_copy(item));
	}

	node_copy->WHILE.expr = deep_copy(node->WHILE.expr);
	node_copy->WHILE.body.clear();
	for (auto expr : node->WHILE.body)
	{
		node_copy->WHILE.body.push_back(deep_copy(expr));
	}

	node_copy->SCOPE.name = node->SCOPE.name;
	node_copy->SCOPE.is_named = node->SCOPE.is_named;
	node_copy->SCOPE.parent = node->SCOPE.parent;
	node_copy->SCOPE.types = node->SCOPE.types;
	node_copy->SCOPE.data.clear();
	for (auto item : node->SCOPE.data)
	{
		node_copy->SCOPE.data.push_back(deep_copy(item));
	}

	return node_copy;
}