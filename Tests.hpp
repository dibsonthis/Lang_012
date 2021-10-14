#pragma once

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"
#include "Utils.hpp"
#include "Eval.hpp"

#include "AST_Node.hpp"
#include "AST_Parser.hpp"
#include "AST_Eval.hpp"

void test_1()
{
	Lexer lexer("source.txt");
	lexer.tokenize();

	print_tokens(lexer.tokens);

	Parser parser(lexer);
	parser.parse();

	std::cin.get();
}

void test_2()
{

	Evaluator eval;

	while (true)
	{
		std::cout << "\n>> ";
		std::string src;
		std::getline(std::cin, src);

		Lexer lexer(src, 0);
		lexer.tokenize();

		Parser parser(lexer);
		parser.parse();

		eval.init(parser);

		if (eval.expressions.size() == 0)
		{
			continue;
		}

		for (auto expr : eval.expressions)
		{
			auto node = eval.evaluate_node(expr);
			std::cout << "\n";
			print_node(node);
			std::cout << "\n";
		}
	}

	std::cin.get();
}

void test_3()
{
	Lexer lexer("source.txt");
	lexer.tokenize();

	Parser parser(lexer);
	parser.parse();

	Evaluator eval(parser);
	eval.debug = false;

	if (eval.expressions.size() == 0)
	{
		return;
	}

	for (auto expr : eval.expressions)
	{
		auto node = eval.evaluate_node(expr);
		//std::cout << "\n";
		//if (node->type == TYPE_ID || node->type == TYPE_INT || node->type == TYPE_FLOAT ||
		//	node->type == TYPE_BOOL || node->type == TYPE_STRING || node->type == TYPE_LIST ||
		//	node->type == TYPE_RANGE || node->type == TYPE_BLOCK ||
		//	node->type == TYPE_IF_ELSE_STATEMENT)
		//{
		//	print_node(node);
		//}
	}

	std::cin.get();
}

void test_4()
{
	Lexer lexer("source.txt");
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
		if (expr->type == TYPE_ERROR)
		{
			std::cout << "\nRunTimeError: Program Exited...";
			std::cin.get();
			exit(1);
		}
	}

	std::cin.get();
}

void ast_node_test()
{
	//std::shared_ptr<AST_Node> scope = std::make_shared<AST_Node>(TYPE_SCOPE);
	//scope->SCOPE.name = "global";
	//
	//auto var_value = std::make_shared<AST_Node>(TYPE_EMPTY);
	//var_value->FLOAT.value = 45.56;

	//auto var = create_var("x", var_value);

	//scope->SCOPE.data.push_back(var);

	//std::shared_ptr<AST_Node> f = std::make_shared<AST_Node>(TYPE_FLOAT);
	//f->FLOAT.value = 46.74;

	//int cast = implicit_cast(var->VAR.value, "custom");

	//auto v = get_var_in_scope("x", scope);
	//auto v2 = create_ref(v);

	//std::cout << (v == var);
	//std::cout << "\n";
	//std::cout << (v == v2);

	//std::cin.get();
}

void ast_parser_test()
{
	Lexer lexer("source.txt");
	lexer.tokenize();

	AST_Parser parser(lexer);
	parser.parse();

	AST_Eval eval(parser);

	eval.init();

	if (parser.expressions.size() == 0)
	{
		std::cin.get();
		return;
	}

	for (auto expr : parser.expressions)
	{
		eval.eval(expr);
		if (expr->type == TYPE_ERROR)
		{
			std::cout << "\nRunTimeError: Program Exited...";
			std::cin.get();
			exit(1);
		}
	}

	std::cin.get();
}