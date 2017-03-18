#pragma once
#ifndef __PARSER_H__
#define __PARSER_H__

#include "thirdparty/script/src/script.h"

class ObjectManager;

class Parser {
public:
	Parser(ObjectManager * object_manager);
	~Parser();

	console_script::Parser * object();

protected:
	void SetupFunctions();

private:
	ObjectManager * object_manager_;
	console_script::Parser parser_;
};

#endif