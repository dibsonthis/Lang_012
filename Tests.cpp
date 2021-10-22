#include "Tests.hpp"

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