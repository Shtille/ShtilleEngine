#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_STORAGE_H__
#define __SHT_SATELLITE_IMAGERY_STORAGE_H__

#include "../include/key_pair.h"
#include "../include/region_info.h"

#include "../../common/non_copyable.h"

#include <string>
#include <vector>
#include <mutex>

namespace sht {
    namespace satellite_imagery {

        // Forward declarations
        class StorageFile;
        class RegionsHeaderFile;

        enum InitializeResult {
            kSuccess,
            kFailed,
            kRegenerated
        };

        //! Structure with information about one storage file
        struct StorageInfo {
            StorageFile * file;    //!< storage file handler
            KeyOffsetMap offsets;  //!< offsets map
            KeyList list;          //!< need to get fast access to a key with lowest counter
        };

        typedef std::map<std::string, StorageInfo> StorageInfoMap;
        typedef std::set<DataKey> DataKeySet;

        //! Data storage interface class
        class Storage : public NonCopyable {
        public:
            explicit Storage(const std::string& provider_hash, const std::string& path);
            virtual ~Storage();

            //! Storage initialization
            bool Initialize();

            //! Called by tile service, finds storage for a key
            KeyPair * GetKeyPair(const DataKey& key, StorageInfo ** storage_info);

            //! Called by tile service, returns region name for a key
            std::string GetKeyRegionName(const DataKey& key);

            //! Called by rendering thread, checks whether key is in any storage
            bool IsExist(const DataKey& key);

            //! Called by tile service, checks whether key is in main storage
            bool IsExistInMain(const DataKey& key);

            //! Called by tile service, loads data from a chosen source
            bool Load(KeyPair * pair, std::string * data, StorageInfo * info);

            //! Called by tile service, saves data to main storage file
            bool SaveMain(const DataKey& key, const std::string * data);

            //! Called by region service, saves data to some named region storage file
            bool SaveSeparate(const DataKey& key, const std::string * data, const std::string& name);

            bool Region_Add(const RegionInfo& region_info);
            bool Region_Rename(const std::string& old_name, const std::string& new_name);
            bool Region_Delete(const std::string& name);
            bool Region_MarkStored(const std::string& name);
            bool Region_MarkInvalid(const std::string& name);

            //! Returns information about region
            bool GetRegionInfo(const std::string& name, StoredRegionInfo& info);
            //! Returns information about regions
            bool GetRegionsInfo(std::vector<StoredRegionInfo>& regions_info);
            //! Returns region file size in bytes
            BigFileSizeType GetRegionFileSize(const std::string& name);

            size_t GetKeyCount() const;

            //! Estimated storage file size for specified number of tiles and average data size
            static BigFileSizeType GetEstimatedFileSize(unsigned int num_tiles, unsigned int data_size);

            //! Maximum storage file size for a region
            static BigFileSizeType GetMaximumRegionFileSize();

        private:
            InitializeResult InitializeFile(StorageInfo& info);
            bool InitializeRegionsHeaderFile();
            void InitializeKeySet();
            void DeinitializeRegions();
            void Deinitialize();
            std::string GenerateRegionFileName();
            bool Save(const DataKey& key, const std::string * data, StorageInfo& info);
            std::string AppendPath(const std::string& filename);

            std::string path_;              //!< stored path value
            unsigned int hash_value_;        //!< stored hash value for selected provider
            std::mutex mutex_;              //!< critical section handle
            std::mutex key_set_mutex_;      //!< critical section handle for key set
            DataKeySet key_set_;             //!< set of keys, done for fast rendering access
            StorageInfo main_info_;          //!< tile service data main file
            RegionsHeaderFile * regions_header_file_;
            RegionMap region_map_;           //!< map with regions information
            StorageInfoMap region_info_map_;  //!< map with regions data information
        };

    } // namespace satellite_imagery
} // namespace sht

#endif