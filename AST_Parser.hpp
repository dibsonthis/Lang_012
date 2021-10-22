#pragma once
#include "Type.hpp"
#include "Token.hpp"
#include "AST_Node.hpp"
#include "Lexer.hpp"

class AST_Parser
{
public:
	std::string file_name;
	std::vector<std::shared_ptr<Token>> tokens;
	int index = 0;
	std::shared_ptr<Token> token = nullptr;

	std::vector<std::string> warnings;
	std::vector<std::string> errors;
	bool has_errors = false;

	bool debug = false;

	std::vector<std::shared_ptr<AST_Node>> expressions;

	AST_Parser() = default;

	AST_Parser(std::vector<std::shared_ptr<Token>>& tokens, std::string file_name);

	AST_Parser(Lexer lexer);

	void advance();

	void backtack();

	std::shared_ptr<Token> peek(int n = 1);

	// ---- Error Handling ---- //

	void error_and_skip_to(Type type, std::shared_ptr<AST_Node> error_node, std::string message);

	void error_and_skip_to(Type type, std::shared_ptr<Token> error_token, std::string message);

	void warn_and_skip_to(Type type, std::shared_ptr<AST_Node> warning_node, std::string message);

	void warn_and_skip_to(Type type, std::shared_ptr<Token> warning_token, std::string message);

	void check_for_warnings();

	void check_for_errors();

	// ---- Parsing ---- //

	std::unordered_map<Type, int> precedence
	{
		{ TYPE_EQUAL,					300 },
		{ TYPE_EQ_AND,					300 },
		{ TYPE_AND,						200 },
		{ TYPE_OR,						200 },
		{ TYPE_RIGHT_ARROW,				180 },
		{ TYPE_RIGHT_ARROW_SINGLE,		180 },
		{ TYPE_COMMA,					170 },
		{ TYPE_PLUS_EQ,					150 },
		{ TYPE_MINUS_EQ,				150 },
		{ TYPE_EQ_EQ,					100 },
		{ TYPE_NOT_EQUAL,				100 },
		{ TYPE_LT_EQUAL,				100 },
		{ TYPE_GT_EQUAL,				100 },
		{ TYPE_LANGLE,					100 },
		{ TYPE_RANGLE,					100 },
		{ TYPE_PLUS,					 40 },
		{ TYPE_MINUS,					 40 },
		{ TYPE_STAR,					 30 },
		{ TYPE_SLASH,					 30 },
		{ TYPE_POS,						 20 },
		{ TYPE_NEG,						 20 },
		{ TYPE_PLUS_PLUS,				  5 },
		{ TYPE_MINUS_MINUS,				  5 },
		{ TYPE_DOT,						  2 },
		{ TYPE_DOUBLE_COLON,			  2 },
		{ TYPE_COLON,					  2 },
		{ TYPE_CALL,					  1 },
		{ TYPE_ID,						  1 },
		{ TYPE_INT,						  1 },
		{ TYPE_FLOAT,					  1 },
		{ TYPE_STRING,					  1 },
		{ TYPE_TYPE,					  1 },
		{ TYPE_RANGE,					  1 },
		{ TYPE_LIST,					  1 },
		{ TYPE_BOOL,					  1 },
	};

	std::shared_ptr<AST_Node> parse_expression(std::vector<std::shared_ptr<AST_Node>> raw_expression);

	void insert_in_ast(std::shared_ptr<AST_Node>& root, std::shared_ptr<AST_Node>& node);

	std::vector<std::shared_ptr<AST_Node>> build_expression();

	std::shared_ptr<AST_Node> parse_atom();

	std::shared_ptr<AST_Node> parse_list_item();

	std::shared_ptr<AST_Node> parse_list();

	std::shared_ptr<AST_Node> parse_return();

	std::shared_ptr<AST_Node> parse_func_def();

	std::shared_ptr<AST_Node> parse_type_def();

	std::shared_ptr<AST_Node> parse_while_loop();

	std::shared_ptr<AST_Node> parse_if_else_atom();

	std::shared_ptr<AST_Node> parse_if_else_statement();

	std::shared_ptr<AST_Node> parse_named_block();

	std::shared_ptr<AST_Node> parse_block();

	std::shared_ptr<AST_Node> parse_paren();

	std::shared_ptr<AST_Node> parse_unary_op();

	std::shared_ptr<AST_Node> parse_arg();

	std::shared_ptr<AST_Node> parse_call();

	void parse();
};
