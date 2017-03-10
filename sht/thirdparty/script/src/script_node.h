#pragma once
#ifndef __CONSOLE_SCRIPT_NODE_H__
#define __CONSOLE_SCRIPT_NODE_H__

#include "script_value.h"

#include <list>

namespace console_script {

	class Node {
		friend class Parser;

	public:
		Node();
		Node(Lexem* lexem);
		Node(Lexem* lexem, Node * parent);
		~Node();

		void Add(Node* node);
		void EvaluateTree();
		void DeleteTree();
		bool CheckTree(String& error);

		Value& data() { return data_; }
		const Value& data() const { return data_; }
		Node * parent() { return parent_; }

		bool IsOperatorBinary(class Operator * op);
		bool IsOperatorBinary();
		bool IsOperatorFormPrefix();

	protected:
		inline void CreationCheck();

	private:
		Value data_;
		Lexem * lexem_;
		Node * parent_;
		std::list<Node*> childs_;
	};

} // namespace console_script

#endif