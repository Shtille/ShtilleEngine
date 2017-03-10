#include "script_node.h"
#include "script_lexem.h"
#include "script_base.h"
#include <assert.h>

namespace console_script {

	Node::Node() :
		data_(), lexem_(nullptr), parent_(nullptr)
	{
	}
	Node::Node(Lexem* lexem) :
		data_(lexem), lexem_(lexem), parent_(nullptr)
	{
		CreationCheck();
	}
	Node::Node(Lexem* lexem, Node * parent) :
		data_(lexem), lexem_(lexem), parent_(parent)
	{
		parent_->Add(this);
		CreationCheck();
	}
	Node::~Node()
	{
		if (lexem_)
			delete lexem_;
	}
	void Node::Add(Node* node)
	{
		childs_.push_back(node);
		node->parent_ = this;
	}
	void Node::EvaluateTree()
	{
		for (std::list<Node*>::iterator it = childs_.begin(); it != childs_.end(); ++it)
			(*it)->EvaluateTree();

		if (lexem_ == nullptr) // root
			return;

		if (lexem_->type == Lexem::kConstant)
		{
			// Evaluated at compile stage
			return;
		}
		else if (lexem_->type == Lexem::kOperator)
		{
			Base& base = Base::GetInstance();
			Operator * op = dynamic_cast<Operator*>(lexem_);
			// Load operator function pointer
			OperatorPtr func = base.GetOperatorPtr(op->info->type()); // lookup
			func(childs_, &data_);
		}
		else if (lexem_->type == Lexem::kFunction)
		{
			Base& base = Base::GetInstance();
			FunctionReference *func_ref = dynamic_cast<FunctionReference*>(lexem_);
			// Load operator function pointer
			std::vector<Variant> args;
			args.resize(childs_.size());
			size_t i = 0;
			for (auto it = childs_.begin(); it != childs_.end(); ++it)
			{
				switch ((*it)->data().type())
				{
				case Value::kBoolean:
					args[i].set<bool>((*it)->data().AsBool());
					break;
				case Value::kInteger:
					args[i].set<int>((*it)->data().AsInteger());
					break;
				case Value::kFloat:
					args[i].set<Float>((*it)->data().AsFloat());
					break;
				case Value::kString:
					args[i].set<String>((*it)->data().AsString());
					break;
				default:
					assert(false && "wrong function argument type");
					break;
				}
				++i;
			}
			base.CallFunction(func_ref->str, args, &data_);
		}
		else if (lexem_->type == Lexem::kVariable)
		{
			Variable *var = dynamic_cast<Variable*>(lexem_);
			data_.Assign(var->info->ptr()); // assign value
		}
	}
	void Node::DeleteTree()
	{
		for (std::list<Node*>::iterator it = childs_.begin(); it != childs_.end(); ++it)
			(*it)->DeleteTree();
		delete this;
	}
	bool Node::CheckTree(String& error)
	{
		for (std::list<Node*>::iterator it = childs_.begin(); it != childs_.end(); ++it)
		if (!(*it)->CheckTree(error))
			return false;

		if (lexem_ == nullptr) // root
			return true;

		assert(lexem_->type != Lexem::kReference && "a reference has been found during tree traverse");

		if (lexem_->type == Lexem::kOperator)
		{
			Operator * op = dynamic_cast<Operator*>(lexem_);
			assert(op);
			// We wont use check functions, just load check information
			if (IsOperatorBinary(op)) // binary
			{
				if (childs_.size() != 2)
				{
					error = CS_TEXT("wrong params count");
					return false;
				}
				std::list<Node*>::const_iterator it = childs_.begin();
				const Value& first = (*it)->data();
				if ((first.type_ & op->info->value_types()) != first.type_)
				{
					error = CS_TEXT("operator ") + op->str + CS_TEXT(" doesn't support this value type");
					return false;
				}
				if ((op->form & Operator::kLValueOnly) && !(*it)->lexem_->is_l_value())
				{
					error = CS_TEXT("first parameter should be lvalue");
					return false;
				}
				const Value& second = (*++it)->data();
				if (first.type() != second.type())
				{
					error = CS_TEXT("parameter type mismatch");
					return false;
				}
				if (op->info->return_type() == Value::kAll)
					data_.set_type(first.type_);
				else
					data_.set_type(op->info->return_type());
			}
			else // unary
			{
				if (childs_.size() != 1)
				{
					error = CS_TEXT("wrong params count");
					return false;
				}
				std::list<Node*>::const_iterator it = childs_.begin();
				const Value& first = (*it)->data();
				if ((first.type_ & op->info->value_types()) != first.type_)
				{
					error = CS_TEXT("operator '") + op->str + CS_TEXT("' doesn't support this value type");
					return false;
				}
				if (op->info->return_type() == Value::kAll)
					data_.set_type(first.type_);
				else
					data_.set_type(op->info->return_type());
			}
		}
		else if (lexem_->type == Lexem::kFunction)
		{
			Base& base = Base::GetInstance();
			FunctionReference * func = dynamic_cast<FunctionReference*>(lexem_);
			assert(func);
			FunctionInfo * info = base.GetFunctionInfo(func->str);
			assert(info);
			// Check function arity
			if (childs_.size() != info->arguments_type().size())
			{
				error = CS_TEXT("wrong arity in function '") + func->str + CS_TEXT("'");
				return false;
			}
			// Set function return type
			switch (info->return_type())
			{
			case Value::kBoolean:
				ValueTypeSetter<bool>::Set(&data_);
				break;
			case Value::kInteger:
				ValueTypeSetter<int>::Set(&data_);
				break;
			case Value::kFloat:
				ValueTypeSetter<Float>::Set(&data_);
				break;
			case Value::kString:
				ValueTypeSetter<String>::Set(&data_);
				break;
			case Value::kVoid:
				break;
			default:
				assert(false && "wrong function return type");
				break;
			}
			// Check function arguments
			size_t i = 0;
			for (auto it = childs_.begin(); it != childs_.end(); ++it)
			{
				if ((*it)->data().type() != info->arguments_type()[i])
				{
					error = CS_TEXT("function argument type mismatch");
					return false;
				}
				++i;
			}
		}
		return true;
	}
	bool Node::IsOperatorBinary(Operator * op)
	{
		return op->is_indefinite_form() ? childs_.size() == 2 : (op->form & Operator::kBinary) != 0;
	}
	bool Node::IsOperatorBinary()
	{
		if (lexem_->type == Lexem::kOperator)
		{
			Operator * op = dynamic_cast<Operator*>(lexem_);
			return op->is_indefinite_form() ? childs_.size() == 2 : (op->form & Operator::kBinary) != 0;
		}
		else
		{
			assert(false);
			return false;
		}
	}
	bool Node::IsOperatorFormPrefix()
	{
		if (lexem_->type == Lexem::kOperator)
		{
			Operator * op = dynamic_cast<Operator*>(lexem_);
			return (op->form & Operator::kUnaryPrefix) == Operator::kUnaryPrefix;
		}
		else
		{
			assert(false);
			return false;
		}
	}
	inline void Node::CreationCheck()
	{
		if (lexem_->type == Lexem::kVariable)
		{
			Variable *var = dynamic_cast<Variable*>(lexem_);
			data_.set_type(var->info->type()); // assign value type
		}
	}

} // namespace script