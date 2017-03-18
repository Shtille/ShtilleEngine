#include "parser.h"

#include "object_manager.h"

Parser::Parser(ObjectManager * object_manager)
: object_manager_(object_manager)
, parser_()
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
	parser_.AddClassFunction("AddSphere", &ObjectManager::AddSphere, object_manager_);

	parser_.AddClassFunction("SetEditorMode", &ObjectManager::set_editor_mode, object_manager_);
}