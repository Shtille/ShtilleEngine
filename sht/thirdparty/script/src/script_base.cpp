#include "script_base.h"
#include "script_func.h"

#include <cstring>
#include <assert.h>

namespace console_script {

	Base * Base::instance_ = nullptr;

	void Base::AddInstance()
	{
		instance_ = new Base();
	}
	void Base::RemoveInstance()
	{
		delete instance_;
	}
	Base& Base::GetInstance()
	{
		assert(instance_);
		return *instance_;
	}
	Base::Base()
	{
		FillOperatorsInfo();
		FillOperatorPtrs();
	}
	void Base::AddOperatorInfo(const String& str, int priority, Operator::Type type, int value_types, int form, Value::Type return_type, bool associativity)
	{
		operator_list_.push_back(str);
		OperatorInfo& info = operators_info_[str];
		info.priority_ = priority;
		info.type_ = type;
		info.value_types_ = value_types;
		info.form_ = form;
		info.return_type_ = return_type;
		info.associativity_ = associativity;
	}
	void Base::FillOperatorsInfo() // based on http://msdn.microsoft.com/en-us/library/x04xhy0h%28v=vs.80%29.aspx and http://en.cppreference.com/w/cpp/language/operator_precedence
	{
		const size_t expected_operators_count = 40;
		operator_list_.reserve(expected_operators_count);
		operators_info_.reserve(expected_operators_count);
		// Additive:
		AddOperatorInfo(CS_TEXT("+"), 12, Operator::kAddition, Value::kInteger | Value::kFloat | Value::kString, Operator::kBinary | Operator::kUnaryPrefix);
		AddOperatorInfo(CS_TEXT("-"), 12, Operator::kSubtraction, Value::kInteger | Value::kFloat, Operator::kBinary | Operator::kUnaryPrefix);
		// Assignment:
		AddOperatorInfo(CS_TEXT("="), 3, Operator::kAssignment, Value::kAll, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("+="), 3, Operator::kAdditionAssignment, Value::kInteger | Value::kFloat | Value::kString, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("-="), 3, Operator::kSubtractionAssignment, Value::kInteger | Value::kFloat, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("*="), 3, Operator::kMultiplicationAssignment, Value::kInteger | Value::kFloat, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("/="), 3, Operator::kDivisionAssignment, Value::kInteger | Value::kFloat, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("|="), 3, Operator::kBitwiseInclusiveOrAssignment, Value::kInteger, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("&="), 3, Operator::kBitwiseAndAssignment, Value::kInteger, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("^="), 3, Operator::kBitwiseExclusiveOrAssignment, Value::kInteger, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("%="), 3, Operator::kModulusAssignment, Value::kInteger, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("<<="), 3, Operator::kLeftShiftAssignment, Value::kInteger, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT(">>="), 3, Operator::kRightShiftAssignment, Value::kInteger, Operator::kBinary | Operator::kLValueOnly, Value::kAll, false);
		// Bitwise:
		AddOperatorInfo(CS_TEXT("&"), 8, Operator::kBitwiseAnd, Value::kInteger);
		AddOperatorInfo(CS_TEXT("^"), 7, Operator::kBitwiseExclusiveOr, Value::kInteger);
		AddOperatorInfo(CS_TEXT("|"), 6, Operator::kBitwiseInclusiveOr, Value::kInteger);
		// Logical:
		AddOperatorInfo(CS_TEXT("&&"), 5, Operator::kLogicalAnd, Value::kBoolean);
		AddOperatorInfo(CS_TEXT("||"), 4, Operator::kLogicalOr, Value::kBoolean);
		// Miscellaneous
		AddOperatorInfo(CS_TEXT(","), 1, Operator::kComma, Value::kAll);
		// Multiplicative:
		AddOperatorInfo(CS_TEXT("*"), 13, Operator::kMultiplication, Value::kInteger | Value::kFloat);
		AddOperatorInfo(CS_TEXT("/"), 13, Operator::kDivision, Value::kInteger | Value::kFloat);
		AddOperatorInfo(CS_TEXT("%"), 13, Operator::kModulus, Value::kInteger);
		// Cast:
		AddOperatorInfo(CS_TEXT("(bool)"), 16, Operator::kCastBoolean, Value::kAll, Operator::kUnaryPrefix, Value::kBoolean);
		AddOperatorInfo(CS_TEXT("(int)"), 16, Operator::kCastInteger, Value::kAll, Operator::kUnaryPrefix, Value::kInteger);
		AddOperatorInfo(CS_TEXT("(float)"), 16, Operator::kCastFloat, Value::kAll, Operator::kUnaryPrefix, Value::kFloat);
		AddOperatorInfo(CS_TEXT("(string)"), 16, Operator::kCastString, Value::kAll, Operator::kUnaryPrefix, Value::kString);
		// Relational and Equality:
		AddOperatorInfo(CS_TEXT("=="), 9, Operator::kEquality, Value::kAll, Operator::kBinary, Value::kBoolean);
		AddOperatorInfo(CS_TEXT("!="), 9, Operator::kNotEqual, Value::kAll, Operator::kBinary, Value::kBoolean);
		AddOperatorInfo(CS_TEXT("<"), 10, Operator::kLessThan, Value::kAll, Operator::kBinary, Value::kBoolean);
		AddOperatorInfo(CS_TEXT(">"), 10, Operator::kGreaterThan, Value::kAll, Operator::kBinary, Value::kBoolean);
		AddOperatorInfo(CS_TEXT("<="), 10, Operator::kLessThanOrEqual, Value::kAll, Operator::kBinary, Value::kBoolean);
		AddOperatorInfo(CS_TEXT(">="), 10, Operator::kGreaterThanOrEqual, Value::kAll, Operator::kBinary, Value::kBoolean);
		// Shift:
		AddOperatorInfo(CS_TEXT("<<"), 11, Operator::kLeftShift, Value::kInteger);
		AddOperatorInfo(CS_TEXT(">>"), 11, Operator::kRightShift, Value::kInteger);
		// Unary:
		AddOperatorInfo(CS_TEXT("!"), 15, Operator::kLogicalNegation, Value::kBoolean, Operator::kUnaryPrefix, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("~"), 15, Operator::kOnesComplement, Value::kInteger, Operator::kUnaryPrefix, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("++"), 15, Operator::kIncrement, Value::kInteger, Operator::kUnaryPrefix | Operator::kUnaryPostfix | Operator::kLValueOnly, Value::kAll, false);
		AddOperatorInfo(CS_TEXT("--"), 15, Operator::kDecrement, Value::kInteger, Operator::kUnaryPrefix | Operator::kUnaryPostfix | Operator::kLValueOnly, Value::kAll, false);
		// Other:
		AddOperatorInfo(CS_TEXT("("), 1, Operator::kAddition, 0);
		AddOperatorInfo(CS_TEXT(")"), 1, Operator::kAddition, 0);
	}
	void Base::FillOperatorPtrs()
	{
		// Binary
		operator_ptrs_[Operator::kAddition]				= &FuncAddition;
		operator_ptrs_[Operator::kSubtraction]			= &FuncSubtraction;
		operator_ptrs_[Operator::kMultiplication]		= &FuncMultiplication;
		operator_ptrs_[Operator::kDivision]				= &FuncDivision;
		operator_ptrs_[Operator::kModulus]				= &FuncModulus;
		operator_ptrs_[Operator::kBitwiseAnd]			= &FuncBitwiseAnd;
		operator_ptrs_[Operator::kBitwiseInclusiveOr]	= &FuncBitwiseInclusiveOr;
		operator_ptrs_[Operator::kBitwiseExclusiveOr]	= &FuncBitwiseExclusiveOr;
		operator_ptrs_[Operator::kLeftShift]			= &FuncLeftShift;
		operator_ptrs_[Operator::kRightShift]			= &FuncRightShift;
		operator_ptrs_[Operator::kLogicalAnd]			= &FuncLogicalAnd;
		operator_ptrs_[Operator::kLogicalOr]			= &FuncLogicalOr;
		operator_ptrs_[Operator::kEquality]				= &FuncEquality;
		operator_ptrs_[Operator::kNotEqual]				= &FuncNotEqual;
		operator_ptrs_[Operator::kLessThan]				= &FuncLessThan;
		operator_ptrs_[Operator::kGreaterThan]			= &FuncGreaterThan;
		operator_ptrs_[Operator::kLessThanOrEqual]		= &FuncLessThanOrEqual;
		operator_ptrs_[Operator::kGreaterThanOrEqual]	= &FuncGreaterThanOrEqual;
		// Unary
		operator_ptrs_[Operator::kLogicalNegation]		= &FuncLogicalNegation;
		operator_ptrs_[Operator::kOnesComplement]		= &FuncOnesComplement;
		operator_ptrs_[Operator::kIncrement]			= &FuncIncrement;
		operator_ptrs_[Operator::kDecrement]			= &FuncDecrement;
		operator_ptrs_[Operator::kCastBoolean]			= &FuncCastBoolean;
		operator_ptrs_[Operator::kCastInteger]			= &FuncCastInteger;
		operator_ptrs_[Operator::kCastFloat]			= &FuncCastFloat;
		operator_ptrs_[Operator::kCastString]			= &FuncCastString;
		// Assignment
		operator_ptrs_[Operator::kAssignment]					= &FuncAssignment;
		operator_ptrs_[Operator::kAdditionAssignment]			= &FuncAdditionAssignment;
		operator_ptrs_[Operator::kSubtractionAssignment]		= &FuncSubtractionAssignment;
		operator_ptrs_[Operator::kMultiplicationAssignment]		= &FuncMultiplicationAssignment;
		operator_ptrs_[Operator::kDivisionAssignment]			= &FuncDivisionAssignment;
		operator_ptrs_[Operator::kModulusAssignment]			= &FuncModulusAssignment;
		operator_ptrs_[Operator::kBitwiseExclusiveOrAssignment] = &FuncBitwiseExclusiveOrAssignment;
		operator_ptrs_[Operator::kBitwiseInclusiveOrAssignment] = &FuncBitwiseInclusiveOrAssignment;
		operator_ptrs_[Operator::kBitwiseAndAssignment]			= &FuncBitwiseAndAssignment;
		operator_ptrs_[Operator::kLeftShiftAssignment]			= &FuncLeftShiftAssignment;
		operator_ptrs_[Operator::kRightShiftAssignment]			= &FuncRightShiftAssignment;
	}
	void Base::CountOperatorMatches(const String& str, MatchInfo& match)
	{
		match.count = 0;
		match.full_match = false;
		size_t n_operators = operator_list_.size();
		for (size_t i = 0u; i < n_operators; ++i)
		{
			const CS_CHAR* s1 = operator_list_[i].c_str();
			const CS_CHAR* s2 = str.c_str();
			size_t c = str.size();
#ifndef PARSER_WIDE_STRING
			if (strncmp(s1, s2, c) == 0)
#else
			if (wcsncmp(s1, s2, c) == 0)
#endif
			{
				++match.count;
				match.full_match = match.full_match || (operator_list_[i] == str);
			}
		}
	}
	bool Base::OperatorExists(const String& str)
	{
		return operators_info_.find(str) != operators_info_.end();
	}
	bool Base::FunctionExists(const String& str)
	{
		return function_ptrs_.find(str) != function_ptrs_.end();
	}
	bool Base::VariableExists(const String& str)
	{
		return variable_ptrs_.find(str) != variable_ptrs_.end();
	}
	void Base::AddVariable(const String& str, void* ptr, Value::Type type)
	{
		variable_ptrs_[str] = VariableInfo(ptr, type);
	}
	OperatorInfo* Base::GetOperatorInfo(const String& str)
	{
		auto it = operators_info_.find(str);
		if (it != operators_info_.end())
			return &(it->second);
		else
			return nullptr;
	}
	VariableInfo* Base::GetVariableInfo(const String& str)
	{
		auto it = variable_ptrs_.find(str);
		if (it != variable_ptrs_.end())
			return &(it->second);
		else
			return nullptr;
	}
	FunctionInfo* Base::GetFunctionInfo(const String& str)
	{
		auto it = function_ptrs_.find(str);
		if (it != function_ptrs_.end())
			return &(it->second);
		else
			return nullptr;
	}
	OperatorPtr Base::GetOperatorPtr(Operator::Type type)
	{
		auto it = operator_ptrs_.find(type);
		assert(it != operator_ptrs_.end());
		return it->second;
	}
	void Base::CallFunction(const String& func_name, std::vector<Variant>& args_vec, Value* ret)
	{
		auto it = function_ptrs_.find(func_name);
		if (it != function_ptrs_.end())
		{
			it->second.func_->Call(ret, args_vec);
		}
	}

} // namespace script