#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_KEY_PAIR_H__
#define __SHT_SATELLITE_IMAGERY_KEY_PAIR_H__

#include "key.h"
#include "data_info.h"

#include <set>
#include <list>

namespace sht {
    namespace satellite_imagery {

        struct StoredKeyPair;

#pragma pack(push, 1)
        struct KeyPair {
            DataKey first;
            DataInfo second;

            KeyPair();
            KeyPair(const DataKey& key); // implicit c-tor
            KeyPair(const StoredKeyPair& stored_pair, FileOffsetType key_offset);
        };
        struct StoredKeyPair {
            PackedKey first;
            StoredDataInfo second;

            StoredKeyPair();
            StoredKeyPair(const KeyPair& pair);
        };
#pragma pack(pop)

        struct KeyMapComparator {
            bool operator() (const KeyPair& lhs, const KeyPair& rhs) {
                return lhs.first < rhs.first;
            }
        };

        typedef std::set<KeyPair, KeyMapComparator> KeyOffsetMap;
        typedef std::list<KeyOffsetMap::iterator> KeyList;

        //! Sort predicate for key list
        bool KeyListSortPredicate(const KeyOffsetMap::iterator& it1, const KeyOffsetMap::iterator& it2);
        
    } // namespace satellite_imagery
} // namespace sht

#endif