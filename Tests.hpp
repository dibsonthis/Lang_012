#pragma once

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Evaluator.hpp"
#include "Utils.hpp"
#include "Eval.hpp"

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