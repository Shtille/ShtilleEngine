#include "script.h"

#include <algorithm>
#include <assert.h>

namespace console_script {

	Parser::Parser()
	{
		Base::AddInstance();
		root_ = new Node();
	}
	Parser::~Parser()
	{
		if (root_)
			root_->DeleteTree();
		for (LexemList::iterator it = elements_.begin(); it != elements_.end(); ++it)
			delete *it;
		Base::RemoveInstance();
	}
	void Parser::Clear()
	{
		if (root_)
			root_->DeleteTree();
		root_ = new Node();
		for (LexemList::iterator it = elements_.begin(); it != elements_.end(); ++it)
			delete *it;
		elements_.clear();
		error_.clear();
	}
	bool Parser::Compile(const String& str)
	{
		// Clean before use
		Clear();
		// Parse a string into elements
		if (!ParseLexems(str))
			return false;
		// Then recognize elements' types
		if (!RecognizeLexems())
			return false;
		// Then build a tree
		if (!BuildTree())
			return false;
		// Then check a tree
		if (!CheckTree())
			return false;

		return true;
	}
	bool Parser::RecognizeLexems()
	{
		Base& base = Base::GetInstance();

		// Check for bracket balance
		int n_brackets = 0;
		for (LexemList::iterator it = elements_.begin(); it != elements_.end(); ++it)
		{
			Lexem * lexem = *it;
			if (lexem->str == CS_TEXT("("))
				++n_brackets;
			else if (lexem->str == CS_TEXT(")"))
				--n_brackets;
			if (n_brackets < 0)
			{
				error_ = CS_TEXT("wrong bracket order");
				return false;
			}
		}
		if (n_brackets % 2 != 0)
		{
			error_ = CS_TEXT("bracket balance is broken");
			return false;
		}

		int i_pos = 0;
		for (LexemList::iterator it = elements_.begin(); it != elements_.end(); ++it)
		{
			Lexem * lexem = *it;

			++i_pos;

			if (lexem->type != Lexem::kUnprocessed)
				continue;

			// Check is it an operation
			if (base.OperatorExists(lexem->str)) // its an operation
			{
				Operator * op = new Operator(lexem->str, i_pos);
				delete lexem;
				*it = op;
				continue;
			}

			// With our new algorithm there is no need to check is it a constant

			// Check is it a registered function
			if (base.FunctionExists(lexem->str)) // function name matches
			{
				LexemList::iterator it_next = it;
				++it_next;
				if (it_next != elements_.end() &&
					!(*it_next)->str.empty() && (*it_next)->str[0] == CS_TEXT('(')) // and has opening bracket
				{
					lexem->type = Lexem::kFunction;
					continue;
				}
			}

			// Check is it a registered variable
			if (base.VariableExists(lexem->str)) // variable name matches
			{
				LexemList::iterator it_next = it;
				++it_next;
				if (it_next == elements_.end() ||
					(*it_next)->str.empty() || (*it_next)->str[0] != CS_TEXT('(')) // and hasn't opening bracket
				{
					Variable * var = new Variable(lexem->str);
					delete lexem;
					*it = var;
					continue;
				}
			}

			// Lexem is unknown
			{
				LexemList::iterator it_next = it;
				++it_next;
				if (it_next != elements_.end() &&
					!(*it_next)->str.empty() && (*it_next)->str[0] == CS_TEXT('('))
				{
					if (Value::IsGoodFunctionName(lexem->str))
					{
						lexem->type = Lexem::kUnknownFunction;
						error_ = CS_TEXT("unknown function ") + lexem->str;
						return false;
					}
				}
				else
				{
					if (Value::IsGoodVariableName(lexem->str))
					{
						lexem->type = Lexem::kUnknownVariable;
						error_ = CS_TEXT("unknown variable ") + lexem->str;
						return false;
					}
				}
				// Lexem is still unknown, seems like error
				error_ = CS_TEXT("syntax error");
				return false;
			}
		}

		// Recognize prefix/postfix operators form
		for (LexemList::iterator it = elements_.begin(); it != elements_.end(); ++it)
		{
			Lexem * lexem = *it;

			if (lexem->type == Lexem::kOperator)
			{
				Operator *op = dynamic_cast<Operator*>(lexem);
				if ((op->form & Operator::kLValueOnly) && ((op->form & Operator::kUnaryPrefix) || (op->form & Operator::kUnaryPostfix))) // are only usable with lvalues
				{
					// Check prefix form first
					LexemList::iterator it_cur = it; ++it_cur;
					if (it_cur != elements_.end() && (*it_cur)->is_l_value())
					{
						op->form = Operator::kUnaryPrefix;
						op->priority = Operator::UnaryPriority();
						op->associativity = Operator::UnaryAssociativity();
						continue;
					}
					// Then check postfix form
					it_cur = it;
					if (it_cur != elements_.begin())
					{
						--it_cur;
						if ((*it_cur)->is_l_value())
						{
							op->form = Operator::kUnaryPostfix;
							op->priority = Operator::UnaryPriority();
							op->associativity = Operator::UnaryAssociativity();
							continue;
						}
					}
					error_ = CS_TEXT("haven't found any suitable lvalue for operator ") + lexem->str;
					return false;
				}
			}
		}

		return true;
	}
	bool Parser::BuildTree()
	{
		std::list< std::list< Lexem* > * > queue;
		std::list< std::pair< std::list< Lexem* > *, Node* > > pending_queue;
		std::list<Lexem*> references_list;
		std::list<Lexem*> *list = new std::list<Lexem*>;
		list->assign(elements_.begin(), elements_.end());
		elements_.clear();

		queue.push_back(list);
		while (!queue.empty())
		{
			list = queue.front();

			bool can_delete_list = true;

			// First search for blocks
			{
				bool is_function = false;
				std::list<Lexem*> *cur_list = nullptr;
				FunctionReference *func_ref = nullptr;
				int num_brackets = 0;
				for (std::list<Lexem*>::iterator it = list->begin(); it != list->end();)
				{
					Lexem *lexem = *it;
					if (lexem->str == CS_TEXT("("))
					{
						if (num_brackets++ == 0) // start of block
						{
							if (is_function) // function block
							{
								delete lexem;
								it = list->erase(it);
								continue;
							}
							else // other block
							{
								// Exchange lexem with reference
								delete lexem;
								Reference *ref = new Reference();
								*it = ref;
								cur_list = ref->list_ptr;
								queue.push_back(cur_list);
								references_list.push_back(ref);
								++it;
								continue;
							}
						}
					}
					else if (lexem->str == CS_TEXT(")"))
					{
						if (--num_brackets == 0) // end of a block
						{
							// Delete current lexem
							delete lexem;
							it = list->erase(it);
							cur_list = nullptr;
							is_function = false;
							continue;
						}
					}
					else if (lexem->type == Lexem::kFunction && !is_function)
					{
						auto it_next = it;
						++it_next;
						if (it_next != list->end() && (*it_next)->str == CS_TEXT("("))
						{
							is_function = true;
							// Exchange lexem with function reference
							func_ref = new FunctionReference(lexem->str);
							delete lexem;
							*it = func_ref;
							if (++it_next != list->end() && (*it_next)->str != CS_TEXT(")")) // function without args (void)
							{
								cur_list = new std::list<Lexem*>();
								queue.push_back(cur_list);
								func_ref->arguments.push_back(cur_list);
							}
							++it;
							continue;
						}
					}
					else if (lexem->str == CS_TEXT(","))
					{
						if (is_function && num_brackets == 1)
						{
							cur_list = new std::list<Lexem*>();
							queue.push_back(cur_list);
							func_ref->arguments.push_back(cur_list);
							delete lexem;
							it = list->erase(it);
							continue;
						}
					}
					if (cur_list) // there is a reference to another list
					{
						// Move lexem to another list
						cur_list->push_back(lexem);
						it = list->erase(it);
						continue;
					}
					++it;
				}
			}

			// Then look for other operations
			int n = std::count_if(list->begin(), list->end(), Lexem::OperatorPredicate);
			if (n == 0) // no operators has been found
			{
				// The only reference
				if (list->size() == 1)
				{
					if (list->front()->type == Lexem::kReference)
					{
						Reference * current_reference = dynamic_cast<Reference*>(list->front());
						if (queue.size() == 1) // parsing complete
						{
							delete current_reference->list_ptr;
							current_reference->list_ptr = nullptr;
							can_delete_list = false;
						}
						else
						{
							// Relink references, if possible
							bool found_reference = false;
							for (auto it_q = queue.begin(); it_q != queue.end(); ++it_q)
							{
								if (*it_q == list) // current list
									continue;
								for (auto it = (*it_q)->begin(); it != (*it_q)->end(); ++it)
								{
									Lexem * lexem = *it;
									if (lexem->type == Lexem::kReference)
									{
										Reference * reffered_lexem = dynamic_cast<Reference*>(lexem);
										if (reffered_lexem->list_ptr == list) // reffered to current list
										{
											// Relink references
											found_reference = true;
											reffered_lexem->list_ptr = current_reference->list_ptr;
											current_reference->list_ptr = nullptr;
											break;
										}
									}
									else if (lexem->type == Lexem::kFunction)
									{
										FunctionReference * func_ref = dynamic_cast<FunctionReference*>(lexem);
										for (auto ita = func_ref->arguments.begin(); ita != func_ref->arguments.end(); ++ita)
											if (*ita == list) // reffered to current list
											{
												// Relink references
												found_reference = true;
												*ita = current_reference->list_ptr;
												current_reference->list_ptr = nullptr;
												break;
											}
										if (found_reference)
											break;
									}
								}
								if (found_reference)
									break;
							}
							if (found_reference) // found it
							{
								can_delete_list = true;
							}
							else // didnt find ref
							{
								// Delay the reference processing
								queue.push_back(list);
								can_delete_list = false;
							}
						}
					}
					else if (list->front()->type == Lexem::kFunction)
					{
						FunctionReference * func_reference = dynamic_cast<FunctionReference*>(list->front());
						if (queue.size() == 1) // parsing complete
						{
							can_delete_list = false;
							Node * node_func = new Node(func_reference); // create node for function
							pending_queue.push_back(std::make_pair(list, node_func));
							for (auto ita = func_reference->arguments.begin(); ita != func_reference->arguments.end();)
							{
#ifdef _DEBUG
								bool found_ref = false;
#endif
								for (auto itp = pending_queue.begin(); itp != pending_queue.end(); ++itp)
									if (itp->first == *ita) // found ref in pending queue
									{
#ifdef _DEBUG
										found_ref = true;
#endif
										// Connect function node directly to the found one
										node_func->Add(itp->second);
										delete itp->first;
										ita = func_reference->arguments.erase(ita);
										pending_queue.erase(itp); // delete it from pending queue
										break;
									}
#ifdef _DEBUG
								if (!found_ref)
									assert(false && "reference is not found during linking");
#endif
							}
						}
						else
						{
							// Relink references, if possible
							size_t n_resolved = 0;
							for (auto ita = func_reference->arguments.begin(); ita != func_reference->arguments.end(); ++ita)
							{
								for (auto itp = pending_queue.begin(); itp != pending_queue.end(); ++itp)
									if (itp->first == *ita) // found ref in pending queue
									{
										++n_resolved;
										break;
									}
							}
							if (n_resolved == func_reference->arguments.size()) // all references could be resolved
							{
								can_delete_list = false;
								Node * node_func = new Node(func_reference); // create node for function
								pending_queue.push_back(std::make_pair(list, node_func));
								for (auto ita = func_reference->arguments.begin(); ita != func_reference->arguments.end();)
								{
									for (auto itp = pending_queue.begin(); itp != pending_queue.end(); ++itp)
										if (itp->first == *ita) // found ref in pending queue
										{
											// Connect function node directly to the found one
											node_func->Add(itp->second);
											delete itp->first;
											ita = func_reference->arguments.erase(ita);
											pending_queue.erase(itp); // delete it from pending queue
											break;
										}
								}
							}
							else // didnt find ref
							{
								// Delay the reference processing
								queue.push_back(list);
								can_delete_list = false;
							}
						}
					}
					else if (list->front()->type == Lexem::kConstant || list->front()->type == Lexem::kVariable)
					{
						can_delete_list = false;
						pending_queue.push_back(std::make_pair(list, new Node(list->front())));
					}
				} // list->size() == 1
			}
			else if (n == 1) // complete
			{
				bool can_build_node = false;
				bool is_ref = std::find_if(list->begin(), list->end(), Lexem::ReferencePredicate) != list->end();
				if (is_ref) // there is any ref
				{
					// Try to link nodes
					if (!pending_queue.empty())
					{
						// First check can we resolve our references
						int i_ref = 0, n_ref = 0;
						for (std::list<Lexem*>::iterator it = list->begin(); it != list->end(); ++it)
						{
							Lexem * lexem = *it;
							if (lexem->type == Lexem::kReference)
							{
								Reference *ref = dynamic_cast<Reference*>(lexem);
								++n_ref;
								for (auto itp = pending_queue.begin(); itp != pending_queue.end(); ++itp)
								{
									if (itp->first == ref->list_ptr)
									{
										++i_ref;
										break;
									}
								}
							}
							else if (lexem->type == Lexem::kFunction)
							{
								FunctionReference *func_ref = dynamic_cast<FunctionReference*>(lexem);
								n_ref += func_ref->arguments.size();
								for (auto ita = func_ref->arguments.begin(); ita != func_ref->arguments.end(); ++ita)
								for (auto itp = pending_queue.begin(); itp != pending_queue.end(); ++itp)
								{
									if (itp->first == *ita)
									{
										++i_ref;
										break;
									}
								}
							}
						}
						if (i_ref == n_ref) // all refs can be resolved
						{
							can_build_node = true;
						}
						else // some refs cann't be resolved
						{
							// We have at least one unresolved reference, so we cann't create a node
							queue.push_back(list); // push current list into the end
							can_delete_list = false;
						}
					}
					else // empty pending queue
					{
						queue.push_back(list);
						can_delete_list = false;
					}
				}
				else // no refs
				{
					can_build_node = true;
				}
				// Build node with childs
				if (can_build_node) // no refs or all refs are resolved
				{
					std::list<Lexem*>::iterator it_cur = std::find_if(list->begin(), list->end(), Lexem::OperatorPredicate);
					std::list<Lexem*>::iterator it_prev = it_cur;
					std::list<Lexem*>::iterator it_next = it_cur;
					bool has_left = it_cur != list->begin() && (*--it_prev)->is_evaluatable();
					bool has_right = *it_cur != list->back() && (*++it_next)->is_evaluatable();
					Operator *op = dynamic_cast<Operator*>(*it_cur);
					Node * node_cur = new Node(op);
					can_delete_list = false;
					pending_queue.push_back(std::make_pair(list, node_cur));
					std::list<Lexem*> arg_list;
					if (op->is_indefinite_form())
					{
						if (has_left && has_right) // two args form (binary)
						{
							arg_list.push_back(*it_prev);
							arg_list.push_back(*it_next);
						}
						else if (has_left) // one arg form (unary)
							arg_list.push_back(*it_prev);
						else
							arg_list.push_back(*it_next);
					}
					else
					{
						if (has_left && (op->form & Operator::kBinary || op->form & Operator::kUnaryPostfix))
						{
							arg_list.push_back(*it_prev);
						}
						if (has_right && (op->form & Operator::kBinary || op->form & Operator::kUnaryPrefix))
						{
							arg_list.push_back(*it_next);
						}
					}
					for (std::list<Lexem*>::iterator it = arg_list.begin(); it != arg_list.end(); ++it)
					{
						Lexem * lexem = *it;
						if (lexem->type == Lexem::kReference) // reference
						{
							Reference *ref = dynamic_cast<Reference*>(lexem);
							auto it_ref = pending_queue.end();
							for (auto itp = pending_queue.begin(); itp != pending_queue.end(); ++itp)
							{
								if (itp->first == ref->list_ptr) // found ref in pending queue
								{
									it_ref = itp;
									break;
								}
							}
							if (it_ref != pending_queue.end()) // found ref
							{
								// Connect node directly to found one
								node_cur->Add(it_ref->second);
								delete ref->list_ptr; // delete list
								ref->list_ptr = nullptr;
								pending_queue.erase(it_ref); // delete it from pending queue
							}
							else // not found ref
							{
								assert(false && "reference is not found during linking");
							}
						}
						else if (lexem->type == Lexem::kFunction)
						{
							FunctionReference * func_ref = dynamic_cast<FunctionReference*>(lexem);
							Node * node_func = new Node(func_ref, node_cur); // create node for function
							for (auto ita = func_ref->arguments.begin(); ita != func_ref->arguments.end(); )
							{
#ifdef _DEBUG
								bool found_ref = false;
#endif
								for (auto itp = pending_queue.begin(); itp != pending_queue.end(); ++itp)
								if (itp->first == *ita) // found ref in pending queue
								{
#ifdef _DEBUG
									found_ref = true;
#endif
									// Connect function node directly to the found one
									node_func->Add(itp->second);
									delete itp->first;
									ita = func_ref->arguments.erase(ita);
									pending_queue.erase(itp); // delete it from pending queue
									break;
								}
#ifdef _DEBUG
								if (!found_ref)
									assert(false && "reference is not found during linking");
#endif
							}
						}
						else
						{
							new Node(lexem, node_cur);
						}
					}
				}
			}
			else // not completed (n > 1)
			{
				bool few_operators = false;
				// Create list of operators to be sorted
				std::list<Lexem*> sorted_list;
				// Fill out our list with operators only
				for (std::list<Lexem*>::iterator it = list->begin(); it != list->end(); ++it)
				{
					Lexem * lexem = *it;
					if (lexem->type == Lexem::kOperator)
					{
						sorted_list.push_back(lexem);
					}
				}
				// Then sort list by priority decrease
				sorted_list.sort(Lexem::SortPredicate);
				// Enumerate operators, sorted by priority decrease
				for (std::list<Lexem*>::iterator it = sorted_list.begin(); it != sorted_list.end(); ++it)
				{
					// Search for it in original list and find pos
					std::list<Lexem*>::iterator it_cur = std::find(list->begin(), list->end(), *it);
					std::list<Lexem*>::iterator it_prev = it_cur;
					std::list<Lexem*>::iterator it_next = it_cur;
					bool has_left = it_cur != list->begin() && (*--it_prev)->is_evaluatable();
					bool has_right = *it_cur != list->back() && (*++it_next)->is_evaluatable();
					if (!has_left && !has_right) // few operators, like ~-1
					{
						few_operators = true;
						continue;
					}
					if (!has_left)
						it_prev = it_cur;
					if (!has_right)
						it_next = it_cur;

					// Make reference
					Reference * ref = new Reference();
					queue.push_back(ref->list_ptr);
					references_list.push_back(ref);

					++it_next; // to point to end
					for (std::list<Lexem*>::iterator it = it_prev; it != it_next; ++it)
					{
						ref->list_ptr->push_back(*it);
					}
					it_cur = list->erase(it_prev, it_next);
					list->insert(it_cur, ref); // insert a new element						
					continue;
				}
				if (few_operators ||
					(list->size() == 1 && list->front()->type == Lexem::kReference))
				{
					queue.push_back(list);
					can_delete_list = false;
				}
			}
			// Finally
			queue.pop_front();
			if (can_delete_list)
				delete list;
			else if (queue.empty())
			{
				// Processing complete
				assert(pending_queue.size() == 1);
				delete list;
				root_->Add(pending_queue.front().second);
				pending_queue.clear();
			}
		}
		// Clear all created references
		for (std::list<Lexem*>::iterator it = references_list.begin(); it != references_list.end(); ++it)
			delete *it;
		return true;
	}
	bool Parser::CheckTree()
	{		
		return root_->CheckTree(error_);
	}
	void Parser::Execute()
	{
		// Assume that tree is checked and all values are good
		root_->EvaluateTree();
	}
	bool Parser::Evaluate(const String& str, int* val)
	{
		if (Compile(str))
		{
			Execute();
			*val = root_->childs_.front()->data_.AsInteger();
			return true;
		}
		else
			return false;
	}
	bool Parser::Evaluate(const String& str, Float* val)
	{
		if (Compile(str))
		{
			Execute();
			*val = root_->childs_.front()->data_.AsFloat();
			return true;
		}
		else
			return false;
	}
	bool Parser::Evaluate(const String& str, String* val)
	{
		if (Compile(str))
		{
			Execute();
			if (root_->childs_.front()->data_.valid()) // we may have a function with returnable type void
				*val = root_->childs_.front()->data_.AsString();
			return true;
		}
		else
			return false;
	}
	void Parser::AddLexem(const String& str, Lexem::Type type, int pos)
	{
		Lexem * lexem;
		if (type == Lexem::kOperator)
		{
			lexem = new Operator(str, pos);
		}
		else
		{
			lexem = new Lexem(str, type);
		}
		elements_.push_back(lexem);
	}
	void Parser::AddVariable(const String& str, bool* ptr)
	{
		Base& base = Base::GetInstance();
		base.AddVariable(str, ptr, Value::kBoolean);
	}
	void Parser::AddVariable(const String& str, int* ptr)
	{
		Base& base = Base::GetInstance();
		base.AddVariable(str, ptr, Value::kInteger);
	}
	void Parser::AddVariable(const String& str, Float* ptr)
	{
		Base& base = Base::GetInstance();
		base.AddVariable(str, ptr, Value::kFloat);
	}
	void Parser::AddVariable(const String& str, String* ptr)
	{
		Base& base = Base::GetInstance();
		base.AddVariable(str, ptr, Value::kString);
	}
}