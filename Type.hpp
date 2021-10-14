#pragma once
#include <vector>
#include <iostream>
#include <unordered_map>

enum Type
{
	TYPE_EMPTY,
	TYPE_EOF,
	TYPE_ERROR,
	TYPE_ID,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_EQUAL,
	TYPE_LPAREN,
	TYPE_RPAREN,
	TYPE_LBRACE,
	TYPE_RBRACE,
	TYPE_LBRACKET,
	TYPE_RBRACKET,
	TYPE_LANGLE,
	TYPE_RANGLE,
	TYPE_DOT,
	TYPE_BACKSLASH,
	TYPE_APOSTROPHE,
	TYPE_EXCLAMATION,
	TYPE_AT,
	TYPE_HASH,
	TYPE_DOLLAR,
	TYPE_CARET,
	TYPE_B_AND,
	TYPE_B_OR,
	TYPE_QUESTION,
	TYPE_PERCENT,
	TYPE_DOUBLE_QUOTE,
	TYPE_MINUS,
	TYPE_PLUS,
	TYPE_SLASH,
	TYPE_STAR,
	TYPE_COMMA,
	TYPE_COLON,
	TYPE_SEMICOLON,
	TYPE_NEG,
	TYPE_POS,

	TYPE_EQ_EQ,
	TYPE_PLUS_EQ,
	TYPE_MINUS_EQ,
	TYPE_PLUS_PLUS,
	TYPE_MINUS_MINUS,
	TYPE_LT_EQUAL,
	TYPE_GT_EQUAL,
	TYPE_NOT_EQUAL,
	TYPE_DOUBLE_COLON,
	TYPE_RIGHT_ARROW,
	TYPE_RIGHT_ARROW_SINGLE,
	TYPE_AND,
	TYPE_OR,

	// Language Specific Types

	TYPE_RAW_EXPR,
	TYPE_PAREN_EXPR,
	TYPE_CALL,
	TYPE_END_OF_EXPRESSON,
	TYPE_BLOCK,
	TYPE_IF,
	TYPE_ELSE,
	TYPE_IF_ELSE_STATEMENT,
	TYPE_WHILE,
	TYPE_TYPE,
	TYPE_TYPE_DEF,
	TYPE_FUNC_DEF,
	TYPE_RETURN,
	TYPE_LIST,
	TYPE_RANGE,
	TYPE_BOOL,
	TYPE_REF,
	TYPE_EQ_AND,
	TYPE_SCOPE,
	TYPE_VAR
};

std::string type_repr(Type type)
{
	switch (type)
	{
		case TYPE_EMPTY:				return "{}";
		case TYPE_ERROR:				return "ERROR";
		case TYPE_EOF:					return "EOF";
		case TYPE_ID:					return "ID";
		case TYPE_INT:					return "INT";
		case TYPE_FLOAT:				return "FLOAT";
		case TYPE_STRING:				return "STRING";
		case TYPE_EQUAL:				return "=";
		case TYPE_LPAREN:				return "(";
		case TYPE_RPAREN:				return ")";
		case TYPE_LBRACE:				return "{";
		case TYPE_RBRACE:				return "}";
		case TYPE_LBRACKET:				return "[";
		case TYPE_RBRACKET:				return "]";
		case TYPE_LANGLE:				return  "<";
		case TYPE_RANGLE:				return  ">";
		case TYPE_DOT:					return ".";
		case TYPE_BACKSLASH:			return "\\";
		case TYPE_APOSTROPHE:			return "'";
		case TYPE_EXCLAMATION:			return "!";
		case TYPE_AT:					return "@";
		case TYPE_HASH:					return "#";
		case TYPE_DOLLAR:				return "$";
		case TYPE_CARET:				return "^";
		case TYPE_B_AND:				return "&";
		case TYPE_B_OR:					return "|";
		case TYPE_QUESTION:				return "?";
		case TYPE_PERCENT:				return "%";
		case TYPE_DOUBLE_QUOTE:			return "\"";
		case TYPE_MINUS:				return "-";
		case TYPE_PLUS:					return "+";
		case TYPE_SLASH:				return "/";
		case TYPE_STAR:					return "*";
		case TYPE_COMMA:				return ":";
		case TYPE_COLON:				return ":";
		case TYPE_SEMICOLON:			return ";";
		case TYPE_NEG:					return "-neg";
		case TYPE_POS:					return "+pos";
 
		case TYPE_EQ_EQ:				return "==";
		case TYPE_PLUS_EQ:				return "+=";
		case TYPE_MINUS_EQ:				return "-=";
		case TYPE_PLUS_PLUS:			return "++";
		case TYPE_MINUS_MINUS:			return "--";
		case TYPE_LT_EQUAL:				return "<=";
		case TYPE_GT_EQUAL:				return ">=";
		case TYPE_NOT_EQUAL:			return "!=";
		case TYPE_DOUBLE_COLON:			return "::";
		case TYPE_RIGHT_ARROW:			return "=>";
		case TYPE_RIGHT_ARROW_SINGLE:	return "->";
		case TYPE_AND:					return "&&";
		case TYPE_OR:					return "||";
		case TYPE_EQ_AND:				return "=&";

		case TYPE_CALL:					return "CALL";
		case TYPE_END_OF_EXPRESSON:		return "EOX";
		case TYPE_BLOCK:				return "BLOCK";
		case TYPE_IF:					return "IF";
		case TYPE_ELSE:					return "ELSE";
		case TYPE_IF_ELSE_STATEMENT:	return "IF_ELSE";
		case TYPE_TYPE:					return "TYPE";
		case TYPE_TYPE_DEF:				return "TYPE_DEF";
		case TYPE_FUNC_DEF:				return "FUNC_DEF";
		case TYPE_RETURN:				return "RETURN";
		case TYPE_LIST:					return "LIST";
		case TYPE_RANGE:				return "RANGE";
		case TYPE_BOOL:					return "BOOL";
		case TYPE_WHILE:				return "WHILE";
		case TYPE_REF:					return "REF";
		case TYPE_SCOPE:				return "SCOPE";
		case TYPE_VAR:					return "VAR";

		default: return "NO_REPR";
	}
}