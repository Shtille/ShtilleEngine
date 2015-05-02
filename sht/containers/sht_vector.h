// sht_vector.h	-- by Vladimir Sviridov <v.shtille@gmail.com> 29 July 2014

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Standart array class container implementation.

#ifndef __SHT_VECTOR_H__
#define __SHT_VECTOR_H__

#include "sht_utility.h"
#include <new>

#pragma pack(push, _CRT_PACKING)

namespace sht {

	// TODO: add reserve() and other functional
	template <typename _Ty1>
	class Vector {
	private:
		typedef _Ty1 ElemType;
		typedef int SizeType;

		inline void _copy(const Vector& v) {
			if (m_buffer_size)
				free(m_buffer);
			m_size = v.m_size;
			m_buffer_size = v.m_buffer_size;
			m_buffer = (ElemType*) malloc(sizeof(ElemType)*m_buffer_size);
			for (SizeType i = 0; i < m_size; ++i)
				m_buffer[i] = v.m_buffer[i];
		}
	public:
		typedef ElemType value_type; // std-like typedef

		// Constructors
		Vector() : m_size(0), m_buffer_size(0), m_buffer(NULL) {
		}
		Vector(SizeType count) : m_size(0), m_buffer_size(0), m_buffer(NULL) {
			resize(count);
		}
		Vector(const Vector& v) {
			_copy(v);
		}
		Vector(const ElemType* array, const SizeType size) {
			m_size = size;
			m_buffer_size = size;
			m_buffer = (ElemType*)malloc(sizeof(ElemType)*m_buffer_size);
			for (SizeType i = 0; i < m_size; ++i)
				m_buffer[i] = array[i];
		}

		// Destructor
		~Vector() {
			if (m_buffer_size)
				free(m_buffer);
		}

		class iterator {
		private:
			ElemType* _ptr;
		public:
			iterator(ElemType* ptr) : _ptr(ptr) {}
			void operator = (ElemType* ptr) {
				_ptr = ptr;
			}
			void operator ++() { // prefix increment
				++_ptr;
			}
			void operator ++(int) { // postfix increment
				++_ptr;
			}
			bool operator == (const iterator& it) {
				return _ptr == it._ptr;
			}
			bool operator != (const iterator& it) {
				return _ptr != it._ptr;
			}
			iterator operator +(int n) {
				return iterator(_ptr + n);
			}
			iterator operator +(unsigned int n) {
				return iterator(_ptr + n);
			}
			iterator operator -(int n) {
				return iterator(_ptr - n);
			}
			iterator operator -(unsigned int n) {
				return iterator(_ptr - n);
			}
			SizeType operator -(ElemType* p) {
				return _ptr - p;
			}
			ElemType& operator *() {
				return *_ptr;
			}
			ElemType* operator ->() {
				return _ptr;
			}
			//      operator ElemType*() {
			//				return _ptr;
			//			}
		};
		iterator begin() {
			return iterator(m_buffer);
		}
		iterator end() {
			return iterator(m_buffer + m_buffer_size);
		}

		void clear() {
			if (m_buffer_size) {
				free(m_buffer);
				m_buffer = NULL;
				m_buffer_size = 0;
				m_size = 0;
			}
		}

		SizeType size() const {
			return m_size;
		}
		bool empty() const {
			return m_size == 0;
		}
		SizeType capacity() const {
			return m_buffer_size;
		}
		ElemType* data() {
			return m_buffer;
		}

		// Operators
		void operator =(const Vector& v) {
			_copy(v);
		}
		ElemType& operator [](const SizeType ind) {
			return m_buffer[ind];
		}
		ElemType& at(const SizeType ind) {
			assert(ind >= 0 && ind < m_size);
			return m_buffer[ind];
		}

		// Resizing
		void resize(SizeType new_size) {
			assert(new_size >= 0);
			SizeType old_size = m_size;
			m_size = new_size;
			// destruct old elements
			for (SizeType i = new_size; i < old_size; ++i)
				(m_buffer + i)->~ElemType();

			if (new_size == 0) {
				m_buffer_size = 0;
			}
			else if (m_size <= m_buffer_size && m_size > m_buffer_size >> 1) {
				// don't compact yet.
				return;
			}
			else {
				m_buffer_size = m_size + (m_size >> 2);
			}

			reserve(m_buffer_size);

			// Copy default T into new elements.
			for (int i = old_size; i < new_size; i++) {
				new (m_buffer + i) ElemType();
			}
		}
		void reserve(SizeType size) {
			assert(size >= 0);
			m_buffer_size = size;
			if (m_buffer_size == 0) {
				if (m_buffer)
					free(m_buffer);
			}
			else {
				if (m_buffer)
					m_buffer = (ElemType*) realloc(m_buffer, sizeof(ElemType)*m_buffer_size);
				else
					m_buffer = (ElemType*) malloc(sizeof(ElemType)*m_buffer_size);
			}
		}
		void push_back() {
			ElemType el;
			SizeType new_size = m_size + 1;
			resize(new_size);
			m_buffer[new_size - 1] = el;
		}
		void push_back(const ElemType& el) {
			SizeType new_size = m_size + 1;
			resize(new_size);
			m_buffer[new_size - 1] = el;
		}
		void pop_back() {
			if (m_size)
				resize(m_size - 1);
		}
		void push_front(const ElemType& el) {
			SizeType new_size = m_size + 1;
			resize(new_size);
			for (SizeType i = new_size-2; i >= 0; --i)
				m_buffer[i + 1] = m_buffer[i];
			m_buffer[0] = el;
		}
		void pop_front() {
			if (m_size)
			{
				SizeType new_size = m_size - 1;
				for (SizeType i = 0; i < new_size; ++i)
					m_buffer[i] = m_buffer[i + 1];
				resize(new_size);
			}
		}
		void insert(iterator where, const ElemType& el) {
			SizeType ind = where - m_buffer;
			SizeType new_size = m_size + 1;
			resize(new_size);
			for (SizeType i = new_size - 2; i >= ind; --i)
				m_buffer[i + 1] = m_buffer[i];
			m_buffer[ind] = el;
		}
		void erase(const SizeType ind) {
			assert(ind >= 0 && ind < m_size);
			SizeType new_size = m_size - 1;
			for (SizeType i = ind; i < new_size; ++i)
				m_buffer[i] = m_buffer[i + 1];
			resize(new_size);
		}
		void erase(iterator where) {
			SizeType ind = where - m_buffer;
			assert(ind >= 0 && ind < m_size);
			SizeType new_size = m_size - 1;
			for (SizeType i = ind; i < new_size; ++i)
				m_buffer[i] = m_buffer[i + 1];
			resize(new_size);
		}

		ElemType& front() {
			return m_buffer[0];
		}
		ElemType& back() {
			return m_buffer[m_size - 1];
		}
		iterator find(const ElemType& el) {
			for (SizeType i = 0; i < m_size; ++i)
				if (m_buffer[i] == el)
					return iterator(m_buffer + i);
			return end();
		}
		void swap(Vector & a) {
			ElemType* buffer = m_buffer;
			SizeType size = m_size;
			SizeType buffer_size = m_buffer_size;

			m_buffer = a.m_buffer;
			m_size = a.m_size;
			m_buffer_size = a.m_buffer_size;

			a.m_buffer = buffer;
			a.m_size = size;
			a.m_buffer_size = buffer_size;
		}

	private:
		ElemType* m_buffer;
		SizeType  m_size;
		SizeType  m_buffer_size;
	};

} // namespace sht

#pragma pack(pop)

#endif