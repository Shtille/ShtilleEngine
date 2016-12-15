#include "storage_file.h"

#include "../../system/include/stream/log_stream.h"

#include <assert.h>

namespace sht {
    namespace satellite_imagery {

        static const unsigned int kFormatSignature = 1397116239;

        /*
        File version changes:
        Version 1.
            Original version.
        Version 2.
            Added corruption byte into the header.
        Version 3.
            Added usage counter into key info.
        Version 4.
            Added provider name hash value.
        Version 5.
            Changed key hash format.
        */
        static const unsigned char kVersion = 5;

        static const FileOffsetType kCorruptionByteOffset = sizeof(unsigned int) + sizeof(unsigned char);
        static const FileOffsetType kHeaderSize =
              sizeof(unsigned int)  // signature
            + sizeof(unsigned char) // version
            + sizeof(unsigned char) // corruption
            + sizeof(int)           // maximum file size
            + sizeof(unsigned int)  // provider name hash value
            + sizeof(unsigned int)  // block capacity
            + sizeof(unsigned int); // blocks count

        static_assert(sizeof(StoredKeyPair) == sizeof(DataKey) + sizeof(StoredDataInfo), "Data sizes mismatch");

        const FileOffsetType StorageFile::GetMaxFileSizeTolerance()
        {
            // 1 Mb of disk space is tolerance for the file
            return 10 * (1 << 20);
        }
        StorageFile::StorageFile(const std::string& filename, unsigned int hash_value,
            FileOffsetType max_file_size)
            : File(filename)
            , max_file_size_(max_file_size)
            , hash_value_(hash_value)
            , block_capacity_(kBlockCapacity)
            , blocks_count_(0U)
            , need_to_shrink_(false)
        {
        }
        StorageFile::~StorageFile()
        {
        }
        BigFileSizeType StorageFile::GetEstimatedFileSize(unsigned int num_tiles, unsigned int data_size)
        {
            unsigned int num_blocks = num_tiles / kBlockCapacity;
            if (num_tiles % kBlockCapacity != 0)
                ++num_blocks;
            BigFileSizeType block_size = static_cast<BigFileSizeType>(sizeof(BlockHeader) + sizeof(BlockKeys));
            BigFileSizeType size = static_cast<BigFileSizeType>(kHeaderSize);
            size += block_size * static_cast<BigFileSizeType>(num_blocks);
            size += static_cast<BigFileSizeType>(num_tiles) * static_cast<BigFileSizeType>(data_size);
            return size;
        }
        void StorageFile::WriteKeyPair(const StoredKeyPair& p)
        {
            if (0 == fwrite(&(p.first), sizeof(DataKey::HashType), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
            if (0 == fwrite(&(p.second), sizeof(StoredDataInfo), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
        }
        void StorageFile::ReadKeyPair(StoredKeyPair *p)
        {
            if (0 == fread(&(p->first), sizeof(DataKey::HashType), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
            if (0 == fread(&(p->second), sizeof(StoredDataInfo), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
        }
        bool StorageFile::Flush()
        {
            if (OpenForWrite())
            {
                operation_successful_ = true;
                blocks_count_ = 0;
                WriteHeader();
                Close();
                return success();
            }
            else
                return false;
        }
        void StorageFile::WriteHeader()
        {
            WriteUint(kFormatSignature);
            WriteByte(kVersion);
            WriteByte(0); // corruption byte
            WriteInt(max_file_size_);
            WriteUint(hash_value_);
            WriteUint(block_capacity_);
            WriteUint(blocks_count_);

            // Check that we've updated header size constant.
            FileOffsetType header_size = Tell();
            assert(header_size == kHeaderSize);
        }
        bool StorageFile::ReadHeader()
        {
            // Read signature
            unsigned int signature;
            ReadUint(&signature);
            // Check signature
            if (signature != kFormatSignature)
            {
                LOG_ERROR("Satellite Imagery: wrong file signature, terminating");
                operation_successful_ = false;
                return false;
            }
            // Read version
            unsigned char version;
            ReadByte(&version);
            // Check version
            if (version < kVersion) // out of date
            {
                LOG_INFO("Satellite Imagery: format version %d is out of date. File will be cleaned.", version);
                Regenerate();
                return false;
            }
            // Read corruption byte
            unsigned char corrupted;
            ReadByte(&corrupted);
            // Check corruption
            if (corrupted)
            {
                LOG_INFO("Satellite Imagery: file is broken. It will be cleaned.");
                Regenerate();
                return false;
            }
            // Read max file size
            ReadInt(&max_file_size_);
            // Read provider hash value
            unsigned int hash_value;
            ReadUint(&hash_value);
            if (hash_value != hash_value_)
            {
                LOG_INFO("Satellite Imagery: file provider differs from chosen. File will be cleaned.");
                Regenerate();
                return false;
            }
            // Read block capacity
            ReadUint(&block_capacity_);
            // Read blocks count
            ReadUint(&blocks_count_);
            return true;
        }
        void StorageFile::WriteAllKeys(const KeyOffsetMap& offsets)
        {
            assert(false);
            //KeyOffsetMap::iterator it = offsets.begin();
            //// Assumed that we've written the header
            //for (unsigned int i = 0; i < blocks_count_; ++i)
            //{
            //    // Write block header
            //    BlockHeader block_header;
            //    block_header.count = 0;
            //    block_header.next_offset = 0;
            //    WriteBlockHeader(&block_header);

            //    FileOffsetType data_offset = Tell() + sizeof(BlockKeys);

            //    for (unsigned int i = 0; i < block_header.count; ++i)
            //    {
            //        KeyPair& pair = *it;

            //        FileOffsetType pair_offset = Tell();

            //        pair.second.key_offset = pair_offset;
            //        pair.second.data_offset = data_offset;

            //        // Write key pair
            //        StoredKeyPair stored_pair(pair);
            //        WriteKeyPair(stored_pair);

            //        // Write key data

            //        ++it;
            //    }

            //    
            //}
        }
        void StorageFile::ReadAllKeys(KeyOffsetMap& offsets, KeyList& list)
        {
            FileOffsetType block_offset = Tell();

            // Assumed that we've read the header
            for (unsigned int j = 0; j < blocks_count_; ++j)
            {
                // Seek to the block position
                OffsetFromBeginning(block_offset);

                // Read block header
                BlockHeader block_header;
                ReadBlockHeader(&block_header);
                block_offset = block_header.next_offset;

                // Read keys
                for (unsigned int i = 0; i < block_header.count; ++i)
                {
                    FileOffsetType pair_offset = Tell();

                    StoredKeyPair stored_pair;
                    ReadKeyPair(&stored_pair);

                    KeyPair pair(stored_pair, pair_offset);
                    std::pair<KeyOffsetMap::iterator, bool> result = offsets.insert(pair);
                    assert(result.second);
                    list.push_back(result.first);
                }
            }
            list.sort(KeyListSortPredicate);
        }
        void StorageFile::WriteBlockHeader(const BlockHeader& block_header)
        {
            WriteUint(block_header.count);
            WriteInt(block_header.next_offset);
        }
        void StorageFile::ReadBlockHeader(BlockHeader *block_header)
        {
            ReadUint(&(block_header->count));
            ReadInt(&(block_header->next_offset));
        }
        void StorageFile::WriteBlockKeys(const BlockKeys& keys)
        {
            if (0 == fwrite(keys.key_offsets, sizeof(keys.key_offsets), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
        }
        void StorageFile::ReadBlockKeys(BlockKeys *keys)
        {
            if (0 == fread(keys->key_offsets, sizeof(keys->key_offsets), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
        }
        StorageFile::SaveResult StorageFile::ReplaceKey(const DataKey& key, const std::string * data,
                KeyOffsetMap& offsets, KeyList& list)
        {
            assert(!list.empty());
            FileSizeType data_size = static_cast<FileSizeType>(data->size());
            // At first we should determine which key is to replace
            bool found_replace = false;
            KeyList::size_type half_list_size = list.size()/2U;
            KeyList::size_type count = 0U;
            KeyList::iterator lit = list.begin();
            for (; lit != list.end(); ++lit)
            {
                if (count > half_list_size)
                {
                    lit = list.begin();
                    break;
                }
                KeyOffsetMap::iterator mit = *lit;
                if (data_size <= mit->second.size)
                {
                    found_replace = true;
                    break;
                }
                ++count;
            }
            KeyOffsetMap::iterator it = *lit;
            //LOG_INFO(("Satellite Imagery: replacing key %llu for key %llu", it->first.hash(), key.hash()));
            // Replace existable key struct
            KeyPair pair;
            pair.first = key;
            pair.second.size = data_size;
            pair.second.counter = 0U;
            pair.second.key_offset = it->second.key_offset;
            if (found_replace)
            {
                LOG_INFO("Satellite Imagery: good replacing");
                OffsetFromBeginning(it->second.data_offset);
                pair.second.data_offset = it->second.data_offset;
            }
            else // !found_replace
            {
                LOG_INFO("Satellite Imagery: bad replacing");
                // Old data hasn't enough space for our new data
                // So we gonna write data at the end of the file
                OffsetFromEnd(0);
                pair.second.data_offset = Tell();
                //need_to_shrink_ = true;
                if (pair.second.data_offset + (FileOffsetType)data_size > 
                    max_file_size_ + GetMaxFileSizeTolerance())
                {
                    // We have reached hard cap, so flush the file
                    return kNeedToFlush;
                }
            }
            WriteArray(data->data(), data->size());
            // Write key pair
            OffsetFromBeginning(pair.second.key_offset);
            StoredKeyPair stored_pair(pair);
            WriteKeyPair(stored_pair);
            // Erase old key from map
            offsets.erase(it);
            // Insert a new key pair to map
            std::pair<KeyOffsetMap::iterator, bool> result = offsets.insert(pair);
            // Delete old key from list
            list.erase(lit);
            // Add a new key to list
            list.push_front(result.first);
            return kSuccess;
        }
        StorageFile::SaveResult StorageFile::SaveLogics(const DataKey& key, const std::string * data,
            KeyOffsetMap& offsets, KeyList& list)
        {
            // Skip file header
            OffsetFromBeginning(kHeaderSize);
            FileOffsetType header_offset = kHeaderSize;

            if (blocks_count_ == 0U)
            {
                FileOffsetType block_header_size = static_cast<FileOffsetType>(sizeof(BlockHeader));
                FileOffsetType block_keys_size = static_cast<FileOffsetType>(sizeof(BlockKeys));
                FileOffsetType data_offset = header_offset + block_header_size + block_keys_size;
                // Prepare out data
                KeyPair pair;
                pair.first = key;
                pair.second.key_offset = header_offset + block_header_size; // cuz 1st key
                pair.second.data_offset = data_offset;
                pair.second.size = static_cast<unsigned int>(data->size());
                pair.second.counter = 0U;
                // Insert pair into the map
                std::pair<KeyOffsetMap::iterator, bool> result = offsets.insert(pair);
                // Add a new key to list
                list.push_front(result.first);
                // Create our first block and write a key in there
                BlockHeader block_header;
                block_header.count = 1U;
                block_header.next_offset = 0;
                WriteBlockHeader(block_header);
                BlockKeys keys;
                memset(&keys, 0, sizeof(keys));
                keys.key_offsets[0].first = key;
                keys.key_offsets[0].second.offset = data_offset;
                keys.key_offsets[0].second.size = static_cast<unsigned int>(data->size());
                keys.key_offsets[0].second.counter = 0U;
                WriteBlockKeys(keys);
                WriteArray(data->data(), data->size());
                // Update blocks count information in header
                ++blocks_count_;
                OffsetFromBeginning(0);
                WriteHeader();
                return kSuccess;
            }
            else // there are created blocks
            {
                FileOffsetType data_size = static_cast<FileOffsetType>(data->size());

                // Try to find first empty spot in each block
                for (unsigned int block = 0; block < blocks_count_; ++block)
                {
                    // Store position
                    FileOffsetType this_block_offset = Tell();

                    // Read count of used keys in this block
                    BlockHeader block_header;
                    ReadBlockHeader(&block_header);

                    // Check whether block is full
                    if (block_header.count == kBlockCapacity)
                    {
                        if (block_header.next_offset == 0) // it's the last block
                        {
                            // We've reached the maximum size of block, create a new one
                            OffsetFromEnd(0); // seek to the end of file
                            FileOffsetType file_size = Tell();
                            FileOffsetType block_header_size = static_cast<FileOffsetType>(sizeof(BlockHeader));
                            FileOffsetType block_keys_size = static_cast<FileOffsetType>(sizeof(BlockKeys));
                            FileOffsetType data_offset = file_size + block_header_size + block_keys_size;
                            if (data_offset + data_size > max_file_size_)
                            {
                                return ReplaceKey(key, data, offsets, list);
                            }
                            else
                            {
                                // Prepare out data
                                KeyPair pair;
                                pair.first = key;
                                pair.second.key_offset = file_size + block_header_size; // cuz 1st key
                                pair.second.data_offset = data_offset;
                                pair.second.size = static_cast<unsigned int>(data->size());
                                pair.second.counter = 0U;
                                // Insert pair into the map
                                std::pair<KeyOffsetMap::iterator, bool> result = offsets.insert(pair);
                                // Add a new key to list
                                list.push_front(result.first);
                                // Write a new key in the new block
                                block_header.count = 1U;
                                block_header.next_offset = 0;
                                WriteBlockHeader(block_header);
                                BlockKeys keys;
                                memset(&keys, 0, sizeof(keys));
                                keys.key_offsets[0].first = key;
                                keys.key_offsets[0].second.offset = data_offset;
                                keys.key_offsets[0].second.size = static_cast<unsigned int>(data->size());
                                keys.key_offsets[0].second.counter = 0U;
                                WriteBlockKeys(keys);
                                WriteArray(data->data(), data->size());
                                // Also we need to store offset to this block in the last block
                                OffsetFromBeginning(this_block_offset);
                                block_header.count = kBlockCapacity;
                                block_header.next_offset = file_size;
                                WriteBlockHeader(block_header);
                                // Update blocks count information in header
                                ++blocks_count_;
                                OffsetFromBeginning(0);
                                WriteHeader();
                                return kSuccess;
                            }
                        }
                        else
                        {
                            // There is another block, goto it
                            OffsetFromBeginning(block_header.next_offset);
                            continue;
                        }
                    }
                    else // block isn't full
                    {
                        assert(block_header.next_offset == 0);
                        // Seek to the end of file to get offset
                        FileOffsetType this_offset = Tell();
                        OffsetFromEnd(0);
                        FileOffsetType data_offset = Tell();
                        OffsetFromBeginning(this_offset);
                        if (data_offset + data_size > max_file_size_)
                        {
                            return ReplaceKey(key, data, offsets, list);
                        }
                        else
                        {
                            // We will use summ of sizes to not use pragma pack
                            FileOffsetType key_displacement = block_header.count * (sizeof(DataKey) + sizeof(StoredDataInfo));
                            // Prepare out data
                            KeyPair pair;
                            pair.first = key;
                            pair.second.key_offset = this_offset + key_displacement;
                            pair.second.data_offset = data_offset;
                            pair.second.size = static_cast<unsigned int>(data->size());
                            pair.second.counter = 0U;
                            // Insert pair into the map
                            std::pair<KeyOffsetMap::iterator, bool> result = offsets.insert(pair);
                            // Add a new key to list
                            list.push_front(result.first);
                            // Write key-offset pair
                            OffsetFromCurrent(key_displacement);
                            StoredKeyPair stored_pair(pair);
                            WriteKeyPair(stored_pair);
                            // Write data
                            OffsetFromBeginning(data_offset);
                            WriteArray(data->data(), data->size());
                            // Now we should increase block header count
                            ++block_header.count;
                            OffsetFromBeginning(this_block_offset);
                            WriteBlockHeader(block_header);
                            return kSuccess;
                        }
                    }
                } // end for
                assert(false);
            }
            return kFailed;
        }
        void StorageFile::MarkOperationsBegin()
        {
            FileOffsetType position = Tell();
            OffsetFromBeginning(kCorruptionByteOffset);
            WriteByte(1);
            OffsetFromBeginning(position);
        }
        void StorageFile::MarkOperationsEnd()
        {
            FileOffsetType position = Tell();
            OffsetFromBeginning(kCorruptionByteOffset);
            WriteByte(0);
            OffsetFromBeginning(position);
        }

    } // namespace satellite_imagery
} // namespace sht