#pragma once
#ifndef __PARSER_H__
#define __PARSER_H__

#include "thirdparty/script/src/script.h"

class Parser {
public:
	Parser();
	~Parser();

	console_script::Parser * object();

protected:
	void SetupFunctions();

private:
	console_script::Parser parser_;
};

#endif