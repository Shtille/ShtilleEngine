#include "../include/key_pair.h"

namespace sht {
    namespace satellite_imagery {

        KeyPair::KeyPair()
        {
        }
        KeyPair::KeyPair(const RawKey& key)
            : first(key)
            , second()
        {
        }
        KeyPair::KeyPair(const StoredKeyPair& stored_pair, FileOffsetType key_offset)
            : first(stored_pair.first)
            , second(key_offset, stored_pair.second)
        {
        }
        StoredKeyPair::StoredKeyPair()
        {
        }
        StoredKeyPair::StoredKeyPair(const KeyPair& pair)
            : first(pair.first)
            , second(pair.second)
        {
        }
        bool KeyListSortPredicate(const KeyOffsetMap::iterator& it1, const KeyOffsetMap::iterator& it2)
        {
            return it1->second.counter < it2->second.counter;
        }
        
    } // namespace satellite_imagery
} // namespace sht