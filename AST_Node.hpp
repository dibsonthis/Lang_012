#pragma once
#include <string>
#include <map>
#include "Type.hpp"
#include "Token.hpp"

struct AST_Node;

struct Int_Node
{
	int value = 0;
	Int_Node() = default;
	Int_Node(int value) : value(value) {}
};

struct Float_Node
{
	float value = 0;
	Float_Node() = default;
	Float_Node(float value) : value(value) {}
};

struct Bool_Node
{
	bool value = 0;
	Bool_Node() = default;
	Bool_Node(bool value) : value(value) {}
};

struct ID_Node
{
	std::string value = "";
	ID_Node() = default;
	ID_Node(std::string value) : value(value) {}
};

struct String_Node
{
	std::string value = "";
	String_Node() = default;
	String_Node(std::string value) : value(value) {}
};

struct Call_Node
{
	std::string name = "";
	std::vector<std::shared_ptr<AST_Node>> args;
};

struct Func_Def_Node
{
	std::string name = "";
	std::vector<std::shared_ptr<AST_Node>> params;
	std::vector<std::shared_ptr<AST_Node>> body;
	std::shared_ptr<AST_Node> return_type = nullptr;
};

struct Block_Node
{
	std::string name = "";
	std::vector<std::shared_ptr<AST_Node>> body;
};

struct If_Node
{
	std::shared_ptr<AST_Node> expr = nullptr;
	std::shared_ptr<AST_Node> body = nullptr;
};

struct If_Statement_Node
{
	std::vector<std::shared_ptr<AST_Node>> statements;
};

struct Ref_Node
{
	std::shared_ptr<AST_Node> ref = nullptr;
};

struct Type_Node
{
	std::string name = "";
	bool built_in = true;
};

struct Var_Node
{
	std::string name = "";
	std::shared_ptr<AST_Node> type = nullptr;
	std::shared_ptr<AST_Node> value = nullptr;
};

struct Type_Def_Node
{
	std::string name = "";
	std::vector<std::shared_ptr<AST_Node>> body;
};

struct Return_Node
{
	std::shared_ptr<AST_Node> value = nullptr;
};

struct List_Node
{
	std::vector<std::shared_ptr<AST_Node>> items;
};

struct While_Node
{
	std::shared_ptr<AST_Node> expr = nullptr;
	std::vector<std::shared_ptr<AST_Node>> body;
};

struct Scope_Node
{
	std::string name = "";
	std::shared_ptr<AST_Node> parent = nullptr;
	bool is_named = false;

	std::vector<std::shared_ptr<AST_Node>> data;
	std::vector<std::string> types;
};

struct AST_Node
{
	int column = 1;
	int line = 1;

	Type type = TYPE_EMPTY;
	std::shared_ptr<AST_Node> left = nullptr;
	std::shared_ptr<AST_Node> right = nullptr;

	bool is_op = false;
	bool is_post_op = false;

	bool is_p_expr = false;
	bool is_list_item = false;

	AST_Node() = default;

	AST_Node(Type type) : type(type) {}

	AST_Node(std::shared_ptr<Token> token) 
		: type(token->type), column(token->column), line(token->line), is_op(token->is_op), is_post_op(token->is_post_op),	INT(token->int_value), FLOAT(token->float_value), BOOL(token->bool_value), ID(token->get_id_value()), 
			STRING(token->get_string_value())
		{}

	Int_Node			INT;
	Float_Node			FLOAT;
	Bool_Node			BOOL;
	ID_Node				ID;
	String_Node			STRING;
	Type_Node			TYPE;
	Ref_Node			REF;
	Var_Node			VAR;
	Block_Node			BLOCK;
	Scope_Node			SCOPE;
	Func_Def_Node		FUNC_DEF;
	Type_Def_Node		TYPE_DEF;
	Call_Node			CALL;
	Return_Node			RETURN;
	List_Node			LIST;
	While_Node			WHILE;
	If_Node				IF;
	If_Statement_Node	IF_STATEMENT;
};

std::shared_ptr<AST_Node> create_ref(std::shared_ptr<AST_Node> node)
{
	return node;
}

std::shared_ptr<AST_Node> create_copy(std::shared_ptr<AST_Node> node)
{
	std::shared_ptr<AST_Node> r = std::make_shared<AST_Node>(*node);
	return r;
}