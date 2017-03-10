#include "parser.h"

Parser::Parser()
: parser_()
{
	SetupFunctions();
}
Parser::~Parser()
{

}
console_script::Parser * Parser::object()
{
	return &parser_;
}
void Parser::SetupFunctions()
{
	//parser_.AddFunction(...);
}