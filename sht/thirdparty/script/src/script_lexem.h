#pragma once
#ifndef __CONSOLE_SCRIPT_LEXEM_H__
#define __CONSOLE_SCRIPT_LEXEM_H__

#include "script_defines.h"
#include "script_value.h"

#include <string>
#include <list>

namespace console_script {

	class Lexem {
	public:
		enum Type {
			kUnprocessed,
			kUnknownFunction,
			kUnknownVariable,
			kOperator,
			kFunction,
			kConstant,
			kVariable,
			kReference
		};

		Lexem();
		Lexem(const String& str, Type type);
		virtual ~Lexem();

		bool is_l_value() const;
		bool is_evaluatable() const;

		static bool SortPredicate(Lexem* first, Lexem* second);
		static bool OperatorPredicate(Lexem* first);
		static bool ReferencePredicate(Lexem* first);

		String str;
		int priority;
		Type type;
	};

	class Operator : public Lexem {
	public:
		enum Type {
			kAddition,						// +
			kSubtraction,					// -
			kAssignment,					// =
			kAdditionAssignment,			// +=
			kSubtractionAssignment,			// -=
			kBitwiseAndAssignment,			// &=
			kBitwiseExclusiveOrAssignment,	// ^=
			kBitwiseInclusiveOrAssignment,	// |=
			kMultiplicationAssignment,		// *=
			kDivisionAssignment,			// /=
			kModulusAssignment,				// %=
			kLeftShiftAssignment,			// <<=
			kRightShiftAssignment,			// >>=
			kBitwiseAnd,					// &
			kBitwiseExclusiveOr,			// ^
			kBitwiseInclusiveOr,			// |
			kLogicalAnd,					// &&
			kLogicalOr,						// ||
			kComma,							// ,
			kSemicolon,						// ;
			kMultiplication,				// *
			kDivision,						// /
			kModulus,						// %
			kEquality,						// ==
			kNotEqual,						// !=
			kGreaterThanOrEqual,			// >=
			kGreaterThan,					// >
			kLessThanOrEqual,				// <=
			kLessThan,						// <
			kLeftShift,						// <<
			kRightShift,					// >>
			kLogicalNegation,				// !
			kOnesComplement,				// ~
			kIncrement,						// ++
			kDecrement,						// --
			kCastBoolean,					// (bool)
			kCastInteger,					// (int)
			kCastFloat,						// (float)
			kCastString,					// (string)
		};
		enum Form {
			kBinary			= 0x01,
			kUnaryPrefix	= 0x02,
			kUnaryPostfix	= 0x04,
			kLValueOnly		= 0x08
		};

		Operator(const String& str, int pos);

		bool is_indefinite_form() const;
		static const int UnaryPriority();
		static const bool UnaryAssociativity();

		int form;
		int pos; //!< for sorting
		bool associativity;
		class OperatorInfo * info;
	};

	class Reference : public Lexem {
	public:
		Reference();
		virtual ~Reference();

		std::list<Lexem*> *list_ptr;
	};

	class FunctionReference : public Lexem {
		typedef std::list<Lexem*> LexemList;

	public:
		FunctionReference(const String& str);
		virtual ~FunctionReference();

		std::list<LexemList*> arguments;
	};

	class Variable : public Lexem {
	public:
		Variable(const String& str);

		class VariableInfo *info;
	};

} // namespace console_script

#endif