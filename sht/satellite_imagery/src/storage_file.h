#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_STORAGE_FILE_H__
#define __SHT_SATELLITE_IMAGERY_STORAGE_FILE_H__

#include "file.h"
#include "../include/key_pair.h"

namespace sht {
    namespace satellite_imagery {

        // Forward declarations:

        //! Data storage interface class
        /*!
        Note: we don't care about endianness, because file is used only on one system.
        File open and close operations has been optimized due to using it only from single thread.
        */
        class StorageFile : public File {
            friend class Storage;
        public:
            explicit StorageFile(const std::string& filename, unsigned int hash_value,
                FileOffsetType max_file_size);
            virtual ~StorageFile();

            static BigFileSizeType GetEstimatedFileSize(unsigned int num_tiles, unsigned int data_size);

            bool Flush();

            void MarkOperationsBegin();
            void MarkOperationsEnd();

            // Write operations
            void WriteHeader();
            void WriteAllKeys(const KeyOffsetMap& offsets);

            // Read operations
            bool ReadHeader();
            void ReadAllKeys(KeyOffsetMap& offsets, KeyList& list);

            enum SaveResult {
                kSuccess,
                kFailed,
                kNeedToFlush
            };

            SaveResult SaveLogics(const DataKey& key, const std::string * data,
                KeyOffsetMap& offsets, KeyList& list);

        protected:

            // Block capacity now is a hardcoded part
            static const unsigned int kBlockCapacity = 100;

            struct BlockHeader {
                unsigned int count;         //!< number of used blocks
                FileOffsetType next_offset; //!< offset to the next block
            };
            struct BlockKeys {
                StoredKeyPair key_offsets[kBlockCapacity];
            };

            // Some tweak functions
            virtual const FileOffsetType GetMaxFileSizeTolerance(); //!< tolerance of maximum file size

            // Some base operations
            void WriteKeyPair(const StoredKeyPair& p);
            void ReadKeyPair(StoredKeyPair *p);
            void WriteBlockHeader(const BlockHeader& block_header);
            void ReadBlockHeader(BlockHeader *block_header);
            void WriteBlockKeys(const BlockKeys& keys);
            void ReadBlockKeys(BlockKeys *keys);
            SaveResult ReplaceKey(const DataKey& key, const std::string * data,
                KeyOffsetMap& offsets, KeyList& list);

            FileOffsetType max_file_size_; // up to 2 GB
            unsigned int hash_value_;
            unsigned int block_capacity_;
            unsigned int blocks_count_;

        private:
            bool need_to_shrink_;
        };

    } // namespace satellite_imagery
} // namespace sht

#endif