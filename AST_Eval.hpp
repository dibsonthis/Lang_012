#pragma once

#include "AST_Parser.hpp"
#include "AST_Utils.hpp"
#include "Type.hpp"

std::string not_implemented_error(std::shared_ptr<AST_Node>& op);

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

	void init();

	std::shared_ptr<AST_Node> infer_type(std::shared_ptr<AST_Node>& node);

	int implicit_cast(std::shared_ptr<AST_Node>& value, std::string type);

	std::shared_ptr<AST_Node> create_var(std::string name, std::shared_ptr<AST_Node> value, 
		std::shared_ptr<AST_Node> type = nullptr);

	std::shared_ptr<AST_Node> get_data(std::string name);

	std::shared_ptr<AST_Node> get_data_from_scope(std::string name, std::shared_ptr<AST_Node> scope);

	void eval(std::shared_ptr<AST_Node>& node);

	void eval_plus(std::shared_ptr<AST_Node>& node);

	void eval_minus(std::shared_ptr<AST_Node>& node);

	void eval_mul(std::shared_ptr<AST_Node>& node);

	void eval_div(std::shared_ptr<AST_Node>& node);

	void eval_neg(std::shared_ptr<AST_Node>& node);

	void eval_pos(std::shared_ptr<AST_Node>& node);

	void eval_eq_check(std::shared_ptr<AST_Node>& node);

	void eval_not_eq_check(std::shared_ptr<AST_Node>& node);

	void eval_assignment(std::shared_ptr<AST_Node>& node);

	void eval_block(std::shared_ptr<AST_Node>& node);

	void eval_scope_accessor(std::shared_ptr<AST_Node>& node);

	std::shared_ptr<AST_Node> new_scope(std::string name = "");

	void enter_scope(std::shared_ptr<AST_Node>& scope);

	void clear_scope(std::shared_ptr<AST_Node>& scope);

	void delete_scope(std::shared_ptr<AST_Node>& scope);

	void exit_scope();

	void eval_id(std::shared_ptr<AST_Node>& node);

	void eval_var(std::shared_ptr<AST_Node>& node);

	void eval_if_else(std::shared_ptr<AST_Node>& node);

	void eval_while(std::shared_ptr<AST_Node>& node);

	void eval_func_def(std::shared_ptr<AST_Node>& node);

	void eval_return(std::shared_ptr<AST_Node>& node);

	void eval_call(std::shared_ptr<AST_Node>& node);

	void call_str(std::shared_ptr<AST_Node>& arg);

	void call_import(std::shared_ptr<AST_Node>& arg);
};