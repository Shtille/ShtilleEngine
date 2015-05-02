// sht_utility.h	-- by Vladimir Sviridov <v.shtille@gmail.com> 29 July 2014

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Header consists of implementetion of variuos useful types for inner use.

#ifndef __SHT_HASH_H__
#define __SHT_HASH_H__

#include <assert.h>
#include "sht_utility.h"
#include <vector>

#pragma pack(push, _CRT_PACKING)

namespace sht {

	template<class T>
	class FixedSizeHash
		// Computes a hash of an object's representation.
	{
	public:
		static int compute(const T& data)
		{
			unsigned char* p = (unsigned char*)&data;
			int	size = sizeof(T);

			// Hash function suggested by http://www.cs.yorku.ca/~oz/hash.html
			// Due to Dan Bernstein.  Allegedly very good on strings.
			int	h = 5381;
			while (size > 0) {
				h = ((h << 5) + h) ^ *p;
				p++;
				size--;
			}

			// Alternative: "sdbm" hash function, suggested at same web page above.
			// h = 0;
			// for bytes { h = (h << 16) + (h << 6) - hash + *p; }

			return h;
		}
	};

	template<class T, class U, class HashFunctor = FixedSizeHash<T> >
	class Hash {
		// Fairly stupid hash table.
		//
		// Never shrinks, unless you explicitly clear() or resize() it.
		// Expands on demand, though.  For best results, if you know roughly
		// how big your table will be, default it to that size when you create
		// it.
	public:
		Hash() { m_entry_count = 0;  m_size_mask = 0; }
		Hash(int size_hint) { m_entry_count = 0; m_size_mask = 0; resize(size_hint); }
		~Hash() {
			clear();
		}

		// @@ need a "remove()" or "set()" function, to replace/remove existing key.

		void	add(T key, U value)
			// Add a new value to the hash table, under the specified key.
		{
			assert(get(key, NULL) == false);

			m_entry_count++;
			check_expand();

			int	hash_value = HashFunctor::compute(key);
			Entry	e;
			e.key = key;
			e.value = value;

			int	index = hash_value & m_size_mask;	// % m_table.size();
			m_table[index].push_back(e);
		}

		void	clear()
			// Remove all entries from the hash table.
		{
			for (int i = 0; i < (int)m_table.size(); i++) {
				m_table[i].clear();
			}
			m_table.clear();
			m_entry_count = 0;
			m_size_mask = 0;
		}


		bool	get(T key, U* value)
			// Retrieve the value under the given key.
			//
			// If there's no value under the key, then return false and leave
			// *value alone.
			//
			// If there is a value, return true, and set *value to the entry's
			// value.
			//
			// If value == NULL, return true or false according to the
			// presence of the key, but don't touch *value.
		{
			if (m_table.size() == 0) {
				return false;
			}

			int	hash_value = HashFunctor::compute(key);
			int	index = hash_value % m_table.size();
			for (int i = 0; i < (int)m_table[index].size(); i++) {
				if (m_table[index][i].key == key) {
					if (value) {
						*value = m_table[index][i].value;
					}
					return true;
				}
			}
			return false;
		}

		void	check_expand()
			// Resize the hash table, if it looks like the size is too small
			// for the current number of entries.
		{
			int	new_size = m_table.size();

			if (m_table.size() == 0 && m_entry_count > 0) {
				// Need to expand.  Let's make the base table size 256
				// elements.
				new_size = 256;
			}
			else if ((int)m_table.size() * 2 < m_entry_count) {
				// Expand.
				new_size = (m_entry_count * 3) >> 1;
			}

			if (new_size != m_table.size()) {
				resize(new_size);
			}
		}


		void	resize(int new_size)
			// Resize the hash table to the given size (Rehash the contents of
			// the current table).
		{
			if (new_size <= 0) {
				// Special case.
				clear();
				return;
			}

			// Force new_size to be a power of two.
			int	bits = (int)(log2((float)(new_size - 1)) + 1);
			assert((1 << bits) >= new_size);

			new_size = 1 << bits;
			m_size_mask = new_size - 1;

			std::vector< std::vector<Entry> >	new_table(new_size);

			// Copy all entries to the new table.
			{for (int i = 0; i < (int)m_table.size(); i++) {
				for (int j = 0; j < (int)m_table[i].size(); j++) {
					Entry&	e = m_table[i][j];
					int	hash_value = HashFunctor::compute(e.key);

					int	index = hash_value & m_size_mask;	// % new_table.size();
					new_table[index].push_back(e);
				}
				m_table[i].clear();
			}}
			m_table.clear();

			// Replace old table data with new table's data.
			m_table.swap(new_table);
		}

	private:
		struct Entry {
			T	key;
			U	value;
		};

		int	m_entry_count;
		int	m_size_mask;
		std::vector< std::vector<Entry> >	m_table;
	};

} // namespace sht

#pragma pack(pop)

#endif