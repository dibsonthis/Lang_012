#include "AST_Parser.hpp"

AST_Parser::AST_Parser(std::vector<std::shared_ptr<Token>>& tokens, std::string file_name) : tokens(tokens), file_name(file_name)
{
	token = tokens[0];
}

AST_Parser::AST_Parser(Lexer lexer) : tokens(lexer.tokens), file_name(lexer.get_file_name())
{
	token = tokens[0];
}

void AST_Parser::advance()
{
	if (token->type == TYPE_EOF)
	{
		return;
	}

	token = tokens[++index];
}

void AST_Parser::backtack()
{
	if (index == 0)
	{
		return;
	}

	token = tokens[--index];
}

std::shared_ptr<Token> AST_Parser::peek(int n)
{
	if ((index + n) > tokens.size() - 1)
	{
		return tokens[tokens.size() - 1];
	}

	if ((index + n) < 0)
	{
		return tokens[0];
	}

	return tokens[index + n];
}

// ---- Error Handling ---- //

void AST_Parser::error_and_skip_to(Type type, std::shared_ptr<AST_Node> error_node, std::string message)
{
	std::string error_message = "[Parser] Syntax Error in '" + file_name + "' @ (" + std::to_string(error_node->line) + ", " + std::to_string(error_node->column) + "): " + message;
	errors.push_back(error_message);

	while (token->type != type && token->type != TYPE_EOF)
	{
		advance();
	}
}

void AST_Parser::error_and_skip_to(Type type, std::shared_ptr<Token> error_token, std::string message)
{
	std::string error_message = "[Parser] Syntax Error in '" + file_name + "' @ (" + std::to_string(error_token->line) + ", " + std::to_string(error_token->column) + "): " + message;
	errors.push_back(error_message);

	while (token->type != type && token->type != TYPE_EOF)
	{
		advance();
	}
}

void AST_Parser::warn_and_skip_to(Type type, std::shared_ptr<AST_Node> warning_node, std::string message)
{
	std::string warning_message = "[Parser] Warning in '" + file_name + "' @ (" + std::to_string(warning_node->line) + ", " + std::to_string(warning_node->column) + "): " + message;
	warnings.push_back(warning_message);

	while (token->type != type && token->type != TYPE_EOF)
	{
		advance();
	}
}

void AST_Parser::warn_and_skip_to(Type type, std::shared_ptr<Token> warning_token, std::string message)
{
	std::string warning_message = "[Parser] Warning in '" + file_name + "' @ (" + std::to_string(warning_token->line) + ", " + std::to_string(warning_token->column) + "): " + message;
	warnings.push_back(warning_message);

	while (token->type != type && token->type != TYPE_EOF)
	{
		advance();
	}
}

void AST_Parser::check_for_warnings()
{
	if (debug)
	{
		if (warnings.size() > 0)
		{
			std::cout << "[Parser] " << std::to_string(warnings.size()) + " warning(s) found.\n";
			for (std::string warning_message : warnings)
			{
				std::cout << warning_message << '\n';
			}
		}
		else
		{
			std::cout << "[Parser] No warnings found.\n";
		}
	}
}

void AST_Parser::check_for_errors()
{
	if (errors.size() > 0)
	{
		has_errors = true;
		std::cout << "[Parser] Parsing unsuccessful - " + std::to_string(errors.size()) + " error(s) found.\n";
		for (std::string error_message : errors)
		{
			std::cout << error_message << '\n';
		}
	}
	else
	{
		if (debug)
			std::cout << "[Parser] Parsing successful - no errors found.\n";
	}
}

// ---- Parsing ---- //

std::shared_ptr<AST_Node> AST_Parser::parse_expression(std::vector<std::shared_ptr<AST_Node>> raw_expression)
{
	#define ADVANCE() node = raw_expression[++index]

	std::shared_ptr<AST_Node> node = raw_expression[0];
	int index = 0;

	std::shared_ptr<AST_Node> root = std::make_shared<AST_Node>(TYPE_EMPTY);

	while (node->type != TYPE_END_OF_EXPRESSON)
	{
		if (node->type == TYPE_ERROR)
		{
			return node;
		}

		insert_in_ast(root, node);

		ADVANCE();
	}

	return root;
}

void AST_Parser::insert_in_ast(std::shared_ptr<AST_Node>& root, std::shared_ptr<AST_Node>& node)
{
	if (root == nullptr)
	{
		root = std::make_shared<AST_Node>(TYPE_EMPTY);
	}

	else if (root->type == TYPE_EMPTY)
	{
		root = node;
	}
	else if (root->is_p_expr)
	{
		if (node->is_post_op)
			node->right = root;
		else
			node->left = root;

		root = node;
	}
	else if (node->is_p_expr)
	{
		if (root->right)
		{
			insert_in_ast(root->right, node);
		}
		else
		{
			root->right = node;
		}
	}
	else if (precedence[node->type] >= precedence[root->type])
	{
		if (node->is_post_op)
			node->right = root;
		else
			node->left = root;

		root = node;
	}
	else
	{
		if (root->right)
		{
			insert_in_ast(root->right, node);
		}
		else
		{
			root->right = node;
		}
	}
}

std::vector<std::shared_ptr<AST_Node>> AST_Parser::build_expression()
{
	std::vector<std::shared_ptr<AST_Node>> expr;

	while (token->type != TYPE_SEMICOLON)
	{
		if (token->type == TYPE_EOF)
		{
			std::shared_ptr<AST_Node> error = std::make_shared<AST_Node>(token);
			error->type = TYPE_ERROR;
			expr.push_back(error);

			error_and_skip_to(TYPE_EOF, error, "Missing ';' - Reached EOF.");
			return expr;
		}

		std::shared_ptr<AST_Node> atom = parse_atom();
		expr.push_back(atom);

		// if something errors and skips to semicolon

		if (token->type == TYPE_SEMICOLON)
		{
			break;
		}

		if (token->type == TYPE_END_OF_EXPRESSON)
		{
			std::shared_ptr<AST_Node> end_of_expression = std::make_shared<AST_Node>(token);
			end_of_expression->type = TYPE_END_OF_EXPRESSON;
			expr.push_back(end_of_expression);

			return expr;
		}

		advance();
	}

	std::shared_ptr<AST_Node> end_of_expression = std::make_shared<AST_Node>(token);
	end_of_expression->type = TYPE_END_OF_EXPRESSON;
	expr.push_back(end_of_expression);

	return expr;
}

std::shared_ptr<AST_Node> AST_Parser::parse_atom()
{
	if (token->type == TYPE_ID && token->get_id_value() == "if")
	{
		std::shared_ptr<AST_Node> node = parse_if_else_statement();
		return node;
	}
	else if (token->type == TYPE_ID && token->get_id_value() == "while")
	{
		std::shared_ptr<AST_Node> node = parse_while_loop();
		return node;
	}
	else if (token->type == TYPE_ID && token->get_id_value() == "type")
	{
		std::shared_ptr<AST_Node> node = parse_type_def();
		return node;
	}
	else if (token->type == TYPE_ID && token->get_id_value() == "def")
	{
		std::shared_ptr<AST_Node> node = parse_func_def();
		return node;
	}
	else if (token->type == TYPE_ID && token->get_id_value() == "return")
	{
		std::shared_ptr<AST_Node> node = parse_return();
		return node;
	}
	else if (token->type == TYPE_ID && token->get_id_value() == "break")
	{
		std::shared_ptr<AST_Node> node = std::make_shared<AST_Node>(TYPE_BREAK);
		return node;
	}
	else if (token->type == TYPE_ID && token->get_id_value() == "break_all")
	{
		std::shared_ptr<AST_Node> node = std::make_shared<AST_Node>(TYPE_BREAK_ALL);
		return node;
	}
	else if (token->type == TYPE_ID && peek()->type == TYPE_LBRACE)
	{
		std::shared_ptr<AST_Node> node = parse_named_block();
		return node;
	}
	else if (token->type == TYPE_LBRACKET)
	{
		std::shared_ptr<AST_Node> node = parse_list();
		return node;
	}
	else if (token->type == TYPE_ID && peek()->type == TYPE_LPAREN)
	{
		std::shared_ptr<AST_Node> node = parse_call();
		return node;
	}
	else if ((token->type == TYPE_MINUS || token->type == TYPE_PLUS) &&
		(peek(-1)->is_op || peek(-1)->type == TYPE_SEMICOLON || peek(-1)->type == TYPE_LPAREN ||
			peek(-1)->type == TYPE_LBRACKET || peek(-1)->type == TYPE_LBRACE ||
			peek(-1)->type == TYPE_END_OF_EXPRESSON || peek(-1)->type == TYPE_COMMA ||
			index == 0))
	{
		std::shared_ptr<AST_Node> node = parse_unary_op();
		return node;
	}
	else if (token->type == TYPE_LPAREN)
	{
		std::shared_ptr<AST_Node> node = parse_paren();
		return node;
	}
	else if (token->type == TYPE_LBRACE)
	{
		std::shared_ptr<AST_Node> node = parse_block();
		return node;
	}
	else
	{
		std::shared_ptr<AST_Node> node = std::make_shared<AST_Node>(token);
		return node;
	}
}

std::shared_ptr<AST_Node> AST_Parser::parse_list_item()
{
	std::shared_ptr<AST_Node> item = std::make_shared<AST_Node>(token);

	std::vector<std::shared_ptr<AST_Node>> raw_expr;

	while (token->type != TYPE_COMMA && token->type != TYPE_RBRACKET)
	{
		if (token->type == TYPE_EOF)
		{
			error_and_skip_to(TYPE_EOF, token, "Missing ']' - Reached EOF.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		std::shared_ptr<AST_Node> atom = parse_atom();
		raw_expr.push_back(atom);
		advance();
	}

	raw_expr.push_back(std::make_shared<AST_Node>(TYPE_END_OF_EXPRESSON));

	item = parse_expression(raw_expr);
	return item;
}

std::shared_ptr<AST_Node> AST_Parser::parse_list()
{
	std::shared_ptr<AST_Node> list = std::make_shared<AST_Node>(token);
	list->type = TYPE_LIST;

	advance();

	while (token->type != TYPE_RBRACKET)
	{
		if (token->type == TYPE_EOF)
		{
			error_and_skip_to(TYPE_EOF, list, "Missing ']' - Reached EOF.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		auto item = parse_list_item();
		item->is_list_item = true;

		list->LIST.items.push_back(item);

		if (token->type == TYPE_RBRACKET)
		{
			break;
		}

		advance();
	}

	//if (list->list_items.size() == 1 && list->list_items[0]->type == TYPE_RIGHT_ARROW_SINGLE)
	//{
	//	list->list_items[0]->type = TYPE_RANGE;
	//	return list->list_items[0];
	//}

	return list;
}

std::shared_ptr<AST_Node> AST_Parser::parse_return()
{
	std::shared_ptr<AST_Node> node = std::make_shared<AST_Node>(token);
	node->type = TYPE_RETURN;
	advance();
	auto raw_expr = build_expression();
	std::shared_ptr<AST_Node> expr = parse_expression(raw_expr);
	node->RETURN.value = expr;

	return node;
}

std::shared_ptr<AST_Node> AST_Parser::parse_func_def()
{
	std::shared_ptr<AST_Node> node = std::make_shared<AST_Node>(token);
	node->type = TYPE_FUNC_DEF;

	advance();
	if (token->type != TYPE_ID)
	{
		error_and_skip_to(TYPE_SEMICOLON, node, "Expected identifier.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	node->FUNC_DEF.name = *token->id_value;

	advance();
	if (token->type != TYPE_LPAREN)
	{
		error_and_skip_to(TYPE_SEMICOLON, node, "Expected '('.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	advance();
	while (token->type != TYPE_RPAREN)
	{
		std::shared_ptr<AST_Node> param = parse_arg();
		node->FUNC_DEF.params.push_back(param);

		if (token->type == TYPE_EOF)
		{
			error_and_skip_to(TYPE_EOF, token, "Missing ')' - Reached EOF.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		if (token->type == TYPE_RPAREN)
		{
			break;
		}

		advance();
	}

	advance();
	if (token->type != TYPE_LBRACE && token->type != TYPE_RIGHT_ARROW)
	{
		error_and_skip_to(TYPE_SEMICOLON, node, "Expected '{' or '=>'.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	if (token->type == TYPE_RIGHT_ARROW)
	{
		std::vector<std::shared_ptr<AST_Node>> raw_expr;

		advance();
		while (token->type != TYPE_LBRACE)
		{
			std::shared_ptr<AST_Node> atom = parse_atom();
			raw_expr.push_back(atom);

			if (token->type == TYPE_EOF)
			{
				error_and_skip_to(TYPE_EOF, node, "Missing '{'. Reached EOF.");
				return std::make_shared<AST_Node>(TYPE_ERROR);
			}

			advance();
		}

		raw_expr.push_back(std::make_shared<AST_Node>(TYPE_END_OF_EXPRESSON));

		std::shared_ptr<AST_Node> expr = parse_expression(raw_expr);

		node->RETURN.value = expr;
	}

	auto block = parse_block();
	for (std::shared_ptr<AST_Node> expr : block->BLOCK.body)
	{
		node->FUNC_DEF.body.push_back(expr);
	}

	return node;
}

std::shared_ptr<AST_Node> AST_Parser::parse_type_def()
{
	advance();
	if (token->type != TYPE_ID)
	{
		error_and_skip_to(TYPE_SEMICOLON, token, "Expected identifier.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	std::shared_ptr<AST_Node> node = std::make_shared<AST_Node>(token);
	node->type = TYPE_TYPE_DEF;
	node->TYPE_DEF.name = *token->id_value;

	advance();
	if (token->type != TYPE_LBRACE)
	{
		error_and_skip_to(TYPE_SEMICOLON, token, "Expected '{'.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	std::shared_ptr<AST_Node> body = parse_block();
	for (std::shared_ptr<AST_Node> expr : body->BLOCK.body)
	{
		node->TYPE_DEF.body.push_back(expr);
	}

	return node;
}

std::shared_ptr<AST_Node> AST_Parser::parse_while_loop()
{
	std::shared_ptr<AST_Node> while_loop = std::make_shared<AST_Node>(TYPE_WHILE);

	advance();
	if (token->type != TYPE_LPAREN)
	{
		error_and_skip_to(TYPE_SEMICOLON, while_loop, "Expected a '('.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	while_loop->WHILE.expr = parse_paren();

	advance();
	if (token->type != TYPE_LBRACE)
	{
		error_and_skip_to(TYPE_SEMICOLON, while_loop, "Expected a '{'.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	std::shared_ptr<AST_Node> while_body = parse_block();
	for (auto& expr : while_body->BLOCK.body)
	{
		while_loop->WHILE.body.push_back(expr);
	}

	return while_loop;
}

std::shared_ptr<AST_Node> AST_Parser::parse_if_else_atom()
{
	std::shared_ptr<AST_Node> if_atom = std::make_shared<AST_Node>(token);
	if_atom->type = TYPE_IF;

	advance();
	if (token->type != TYPE_LPAREN)
	{
		error_and_skip_to(TYPE_SEMICOLON, if_atom, "Expected a '('.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	std::shared_ptr<AST_Node> if_expr = parse_paren();
	if_atom->IF.expr = if_expr;

	advance();
	if (token->type != TYPE_LBRACE)
	{
		error_and_skip_to(TYPE_SEMICOLON, if_atom, "Expected a '{'.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	std::shared_ptr<AST_Node> if_body = parse_block();
	if_atom->IF.body = if_body;

	return if_atom;
}

std::shared_ptr<AST_Node> AST_Parser::parse_if_else_statement()
{
	std::shared_ptr<AST_Node> if_statement = std::make_shared<AST_Node>(token);
	if_statement->type = TYPE_IF_ELSE_STATEMENT;

	std::shared_ptr<AST_Node> if_atom = parse_if_else_atom();

	if_statement->IF_STATEMENT.statements.push_back(if_atom);

	while (peek()->get_id_value() == "else" && peek(2)->get_id_value() == "if")
	{
		advance();
		advance();
		std::shared_ptr<AST_Node> if_else_atom = parse_if_else_atom();
		if_statement->IF_STATEMENT.statements.push_back(if_else_atom);
	}

	if (peek()->get_id_value() == "else")
	{
		std::shared_ptr<AST_Node> else_atom = std::make_shared<AST_Node>(token);
		else_atom->type = TYPE_ELSE;

		advance();
		advance();

		if (token->type != TYPE_LBRACE)
		{
			error_and_skip_to(TYPE_SEMICOLON, else_atom, "Expected a '{'.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		else_atom->IF.body = parse_block();
		if_statement->IF_STATEMENT.statements.push_back(else_atom);
	}

	if (peek()->get_id_value() == "else")
	{
		error_and_skip_to(TYPE_SEMICOLON, peek(), "If/Else Statements cannot have more than one Else block.");
		return std::make_shared<AST_Node>(TYPE_ERROR);
	}

	return if_statement;
}

std::shared_ptr<AST_Node> AST_Parser::parse_named_block()
{
	std::string block_name = std::make_shared<AST_Node>(token)->ID.value;
	advance();
	std::shared_ptr<AST_Node> block = std::make_shared<AST_Node>(token);
	block->type = TYPE_BLOCK;
	block->BLOCK.name = block_name;
	advance();

	while (token->type != TYPE_RBRACE)
	{
		auto raw_expr = build_expression();
		std::shared_ptr<AST_Node> expr = parse_expression(raw_expr);

		if (expr->type != TYPE_ERROR)
			block->BLOCK.body.push_back(expr);

		if (token->type == TYPE_EOF)
		{
			error_and_skip_to(TYPE_EOF, token, "Missing '}' - Reached EOF.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		advance();
	}

	token->type = TYPE_END_OF_EXPRESSON;

	return block;
}

std::shared_ptr<AST_Node> AST_Parser::parse_block()
{
	std::shared_ptr<AST_Node> block = std::make_shared<AST_Node>(token);
	block->type = TYPE_BLOCK;
	advance();

	while (token->type != TYPE_RBRACE)
	{
		auto raw_expr = build_expression();
		std::shared_ptr<AST_Node> expr = parse_expression(raw_expr);

		if (expr->type != TYPE_ERROR)
			block->BLOCK.body.push_back(expr);

		if (token->type == TYPE_EOF)
		{
			error_and_skip_to(TYPE_EOF, token, "Missing '}' - Reached EOF.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		advance();
	}

	token->type = TYPE_END_OF_EXPRESSON;

	return block;
}

std::shared_ptr<AST_Node> AST_Parser::parse_paren()
{
	advance();

	std::vector<std::shared_ptr<AST_Node>> raw_expr;

	while (token->type != TYPE_RPAREN)
	{
		if (token->type == TYPE_EOF)
		{
			error_and_skip_to(TYPE_EOF, token, "Missing ')' - Reached EOF.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		if (token->type == TYPE_SEMICOLON)
		{
			advance();
			error_and_skip_to(TYPE_SEMICOLON, token, "Expected ')'. Cannot have ';' inside parentheses.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		if (token->type == TYPE_LPAREN)
		{
			std::shared_ptr<AST_Node> p_exp = parse_paren();
			raw_expr.push_back(p_exp);
			advance();
			continue;
		}

		std::shared_ptr<AST_Node> atom = parse_atom();
		raw_expr.push_back(atom);

		advance();
	}

	std::shared_ptr<AST_Node> end_of_expression = std::make_shared<AST_Node>(TYPE_END_OF_EXPRESSON);
	raw_expr.push_back(end_of_expression);

	std::shared_ptr<AST_Node> node = parse_expression(raw_expr);
	node->is_p_expr = true;
	return node;
}

std::shared_ptr<AST_Node> AST_Parser::parse_unary_op()
{
	std::shared_ptr<AST_Node> node = std::make_shared<AST_Node>(token);

	if (node->type == TYPE_PLUS)
		node->type = TYPE_POS;
	else
		node->type = TYPE_NEG;

	advance();

	std::shared_ptr<AST_Node> right;

	if (token->type == TYPE_LPAREN)
	{
		right = parse_paren();
	}
	else
	{
		right = parse_atom();
	}

	node->right = right;

	return node;
}

std::shared_ptr<AST_Node> AST_Parser::parse_arg()
{
	std::vector<std::shared_ptr<AST_Node>> raw_expr;

	while (token->type != TYPE_COMMA && token->type != TYPE_RPAREN)
	{
		if (token->type == TYPE_EOF)
		{
			error_and_skip_to(TYPE_EOF, token, "Missing ')' - Reached EOF.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		std::shared_ptr<AST_Node> atom = parse_atom();
		raw_expr.push_back(atom);
		advance();
	}

	raw_expr.push_back(std::make_shared<AST_Node>(TYPE_END_OF_EXPRESSON));

	std::shared_ptr<AST_Node> expr = parse_expression(raw_expr);
	return expr;
}

std::shared_ptr<AST_Node> AST_Parser::parse_call()
{
	std::shared_ptr<AST_Node> node = std::make_shared<AST_Node>(token);
	node->type = TYPE_CALL;
	node->CALL.name = node->ID.value;

	advance();
	advance();

	while (token->type != TYPE_RPAREN)
	{
		std::shared_ptr<AST_Node> arg = parse_arg();
		node->CALL.args.push_back(arg);

		if (token->type == TYPE_EOF)
		{
			error_and_skip_to(TYPE_EOF, token, "Missing ')' - Reached EOF.");
			return std::make_shared<AST_Node>(TYPE_ERROR);
		}

		if (token->type == TYPE_RPAREN)
		{
			break;
		}

		advance();
	}

	return node;
}

void AST_Parser::parse()
{
	while (token->type != TYPE_EOF)
	{
		std::vector<std::shared_ptr<AST_Node>> raw_expr = build_expression();
		std::shared_ptr<AST_Node> expr = parse_expression(raw_expr);
		if (expr->type != TYPE_ERROR && expr->type != TYPE_EMPTY)
			expressions.push_back(expr);
		advance();
	}

	check_for_warnings();
	check_for_errors();
}