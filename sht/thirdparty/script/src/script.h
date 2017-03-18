// script.h -- by Vladimir Sviridov <v.shtille@gmail.com> 22 November 2014

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

/* Interface header
	Define PARSER_HIGHP_FLOAT before including this file 
	to make double precision floating point numbers.
*/
#pragma once
#ifndef __CONSOLE_SCRIPT_H__
#define __CONSOLE_SCRIPT_H__

#include "script_defines.h"
#include "script_lexem.h"
#include "script_base.h"

namespace console_script {

	class Parser {
		typedef std::list<Lexem*> LexemList;

	public:
		Parser();
		~Parser();

		bool Compile(const String& str);
		void Execute();
		bool Evaluate(const String& str, int* val);
		bool Evaluate(const String& str, Float* val);
		bool Evaluate(const String& str, String* val);

		void AddVariable(const String& str, bool* ptr);
		void AddVariable(const String& str, int* ptr);
		void AddVariable(const String& str, Float* ptr);
		void AddVariable(const String& str, String* ptr);

		template <typename R, typename... Args>
		void AddFunction(const String& str, R(*func)(Args...)) {
			Base& base = Base::GetInstance();
			base.AddFunction<R, Args...>(str, func);
		}
		template <typename R, typename C, typename... Args>
		void AddClassFunction(const String& str, R(C::*func)(Args...), C * object) {
			Base& base = Base::GetInstance();
			base.AddClassFunction<R, C, Args...>(str, func, object);
		}

		const String& error() const { return error_; }

	private:
		void Clear();
		void AddLexem(const String& str, Lexem::Type type, int pos);
		bool ParseLexems(const String& str);
		bool RecognizeLexems();
		bool BuildTree();
		bool CheckTree();

		LexemList elements_;	//!< elements list
		String error_;		//!< error message of parsed text
		class Node * root_;
	};

} // namespace console_script

#endif