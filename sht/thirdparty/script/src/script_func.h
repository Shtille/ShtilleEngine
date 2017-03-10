#pragma once
#ifndef __CONSOLE_SCRIPT_FUNC_H__
#define __CONSOLE_SCRIPT_FUNC_H__

#include "script_lexem.h"
#include "script_node.h"

namespace console_script {

	// Operator functions
	void FuncAddition(const std::list<Node*>& list, Value* value);
	void FuncSubtraction(const std::list<Node*>& list, Value* value);
	void FuncMultiplication(const std::list<Node*>& list, Value* value);
	void FuncDivision(const std::list<Node*>& list, Value* value);
	void FuncModulus(const std::list<Node*>& list, Value* value);
	void FuncBitwiseAnd(const std::list<Node*>& list, Value* value);
	void FuncBitwiseInclusiveOr(const std::list<Node*>& list, Value* value);
	void FuncBitwiseExclusiveOr(const std::list<Node*>& list, Value* value);
	void FuncLogicalAnd(const std::list<Node*>& list, Value* value);
	void FuncLogicalOr(const std::list<Node*>& list, Value* value);
	void FuncEquality(const std::list<Node*>& list, Value* value);
	void FuncNotEqual(const std::list<Node*>& list, Value* value);
	void FuncLessThan(const std::list<Node*>& list, Value* value);
	void FuncLessThanOrEqual(const std::list<Node*>& list, Value* value);
	void FuncGreaterThan(const std::list<Node*>& list, Value* value);
	void FuncGreaterThanOrEqual(const std::list<Node*>& list, Value* value);
	void FuncLeftShift(const std::list<Node*>& list, Value* value);
	void FuncRightShift(const std::list<Node*>& list, Value* value);
	void FuncLogicalNegation(const std::list<Node*>& list, Value* value);
	void FuncOnesComplement(const std::list<Node*>& list, Value* value);
	void FuncIncrement(const std::list<Node*>& list, Value* value);
	void FuncDecrement(const std::list<Node*>& list, Value* value);
	void FuncCastBoolean(const std::list<Node*>& list, Value* value);
	void FuncCastInteger(const std::list<Node*>& list, Value* value);
	void FuncCastFloat(const std::list<Node*>& list, Value* value);
	void FuncCastString(const std::list<Node*>& list, Value* value);
	void FuncAssignment(const std::list<Node*>& list, Value* value);
	void FuncAdditionAssignment(const std::list<Node*>& list, Value* value);
	void FuncSubtractionAssignment(const std::list<Node*>& list, Value* value);
	void FuncMultiplicationAssignment(const std::list<Node*>& list, Value* value);
	void FuncDivisionAssignment(const std::list<Node*>& list, Value* value);
	void FuncModulusAssignment(const std::list<Node*>& list, Value* value);
	void FuncBitwiseInclusiveOrAssignment(const std::list<Node*>& list, Value* value);
	void FuncBitwiseExclusiveOrAssignment(const std::list<Node*>& list, Value* value);
	void FuncBitwiseAndAssignment(const std::list<Node*>& list, Value* value);
	void FuncLeftShiftAssignment(const std::list<Node*>& list, Value* value);
	void FuncRightShiftAssignment(const std::list<Node*>& list, Value* value);

} // namespace console_script

#endif