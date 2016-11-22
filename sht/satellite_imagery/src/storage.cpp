#include "storage.h"

#include "storage_file.h"
#include "regions_header_file.h"

#include "../../system/include/stream/log_stream.h"
#include "../../system/include/filesystem/directory.h"
#include "../../system/include/filesystem/file_operations.h"

#include <functional>
#include <assert.h>

namespace {
    const sht::satellite_imagery::FileOffsetType kMaxMainFileSize = 300 << 20; // 300 MB
    const sht::satellite_imagery::FileOffsetType kMaxRegionFileSize = 500 << 20; // 500 MB
}

namespace sht {
    namespace satellite_imagery {

        Storage::Storage(const std::string& provider_hash, const std::string& path)
        : path_(path)
        {
			std::hash<std::string> hash_function;
            hash_value_ = static_cast<unsigned int>(hash_function(provider_hash));
        }
        Storage::~Storage()
        {
            Deinitialize();
        }
        InitializeResult Storage::InitializeFile(StorageInfo& info)
        {
            // Read storage setting from header and initialize the key map
            if (info.file->OpenForRead())
            {
                if (info.file->ReadHeader())
                {
                    if (!info.file->success())
                    {
                        info.file->Close();
                        return kFailed;
                    }
                    // Initialize key map
                    info.file->ReadAllKeys(info.offsets, info.list);
                    info.file->Close();
                    return kSuccess;
                }
                else if (info.file->success())
                {
                    // The file has been regenerated
                    info.file->Close();
                    return kRegenerated;
                }
                else
                {
                    // File regeneration has been failed
                    info.file->Close();
                    return kFailed;
                }
            }
            else if (info.file->OpenForWrite())
            {
                LOG_INFO("satellite imagery: storage initialization");
                // Seems like file doesn't exist, let's create a new one
                info.file->WriteHeader();
                if (!info.file->success())
                {
                    info.file->Close();
                    return kFailed;
                }
                // There are no data yet, so we won't fill our map.
                info.file->Close();
                return kSuccess;
            }
            else
                return kFailed;
        }
        bool Storage::InitializeRegionsHeaderFile()
        {
            // Read storage setting from header and initialize the key map
            if (regions_header_file_->OpenForRead())
            {
                if (regions_header_file_->ReadHeader())
                {
                    if (!regions_header_file_->success())
                    {
                        regions_header_file_->Close();
                        return false;
                    }
                    // Initialize key map
                    regions_header_file_->ReadAllRegions(region_map_);
                    regions_header_file_->Close();
                    return true;
                }
                else if (regions_header_file_->success())
                {
                    // The file has been regenerated
                    regions_header_file_->Close();
                    return true;
                }
                else
                {
                    // File regeneration has been failed
                    regions_header_file_->Close();
                    return false;
                }
            }
            else if (regions_header_file_->OpenForWrite())
            {
                LOG_INFO("satellite imagery: regions header file initialization");
                // Seems like file doesn't exist, let's create a new one
                regions_header_file_->WriteHeader();
                if (!regions_header_file_->success())
                {
                    regions_header_file_->Close();
                    return false;
                }
                // There are no data yet, so we won't fill our map.
                regions_header_file_->Close();
                return true;
            }
            else
                return false;
        }
        void Storage::InitializeKeySet()
        {
            key_set_.clear();
            // From main storage
            for (KeyOffsetMap::iterator it = main_info_.offsets.begin(); it != main_info_.offsets.end(); ++it)
            {
                key_set_.insert(it->first);
            }
            // From regions storages
            for (StorageInfoMap::iterator it = region_info_map_.begin(); it != region_info_map_.end(); ++it)
            {
                StorageInfo& info = it->second;
                for (KeyOffsetMap::iterator itk = info.offsets.begin(); itk != info.offsets.end(); ++itk)
                {
                    key_set_.insert(itk->first);
                }
            }
        }
        bool Storage::Initialize()
        {
            regions_header_file_ = new RegionsHeaderFile(AppendPath("regions.omrh"));
            main_info_.file = new StorageFile(AppendPath("data.om"), hash_value_, kMaxMainFileSize);
            if (InitializeFile(main_info_) == kFailed) // offsets & list
                return false;
            if (!InitializeRegionsHeaderFile())
                return false;
            // Initialize region files
            for (RegionMap::iterator it = region_map_.begin(); it != region_map_.end(); ++it)
            {
                const std::string& region_name = it->first;
                StoredRegionInfo& region_info = it->second;
                StorageFile * region_file = new StorageFile(region_info.filename, hash_value_, kMaxRegionFileSize);
                StorageInfo& storage_info = region_info_map_[region_name];
                storage_info.file = region_file;
                InitializeResult result = InitializeFile(storage_info);
                if (result == kFailed) // offsets & list
                    return false;
                if (result == kRegenerated) // region storage file has been corrupted
                {
                    LOG_INFO("satellite imagery: region '%s' storage file has been broken", region_name.c_str());
                    region_info.status = RegionStatus::kInvalid;
                    // Update header file information
                    if (!regions_header_file_->Update(region_map_))
                        return false;
                }
                if (region_info.status == RegionStatus::kInvalid)
                {
                    LOG_INFO("satellite imagery: region '%s' is invalid", region_name.c_str());
                }
            }
            InitializeKeySet();
            return true;
        }
        void Storage::DeinitializeRegions()
        {
            region_map_.clear();
            for (StorageInfoMap::iterator it = region_info_map_.begin(); it != region_info_map_.end(); ++it)
            {
                StorageFile * file = it->second.file;
                delete file;
            }
            region_info_map_.clear();
        }
        void Storage::Deinitialize()
        {
            DeinitializeRegions();
            delete main_info_.file;
            delete regions_header_file_;
        }
        std::string Storage::GenerateRegionFileName()
        {
            unsigned int id = regions_header_file_->GetUniqueID();
            std::string filename;
            filename.resize(30);
            sprintf(&filename[0], "%u.omr", id);
            return AppendPath(filename);
        }
        KeyPair * Storage::GetKeyPair(const DataKey& key, StorageInfo ** storage_info)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            // First time search in the main storage file
            KeyOffsetMap::iterator it = main_info_.offsets.find(key);
            if (it != main_info_.offsets.end())
            {
                *storage_info = &main_info_; // specify main data source
                return (KeyPair*)(&*it);
            }
            else // Try to find this key in regions
            {
                for (StorageInfoMap::iterator it = region_info_map_.begin(); it != region_info_map_.end(); ++it)
                {
                    StorageInfo * info = &(it->second);
                    KeyOffsetMap::iterator itr = info->offsets.find(key);
                    if (itr != info->offsets.end())
                    {
                        *storage_info = info; // specify region data source
                        return (KeyPair*)(&*itr);
                    }
                }
                return 0;
            }
        }
        std::string Storage::GetKeyRegionName(const DataKey& key)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            // Try to find this key in regions
            for (StorageInfoMap::iterator it = region_info_map_.begin(); it != region_info_map_.end(); ++it)
            {
                StorageInfo * info = &(it->second);
                KeyOffsetMap::iterator itr = info->offsets.find(key);
                if (itr != info->offsets.end())
                {
                    return it->first;
                }
            }
            return std::string();
        }
        bool Storage::IsExist(const DataKey& key)
        {
            std::lock_guard<std::mutex> guard(key_set_mutex_);
            return key_set_.find(key) != key_set_.end();
        }
        bool Storage::IsExistInMain(const DataKey& key)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            return main_info_.offsets.find(key) != main_info_.offsets.end();
        }
        bool Storage::Load(KeyPair * pair, std::string * data, StorageInfo * info)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            if (info->file->OpenForRead())
            {
                // Increase counter of found element in the map
                ++pair->second.counter;
                // List contains iterators to map, so we just should sort it
                info->list.sort(KeyListSortPredicate);
                // We should update counter value in the file too
                StoredKeyPair stored_pair(*pair);
                info->file->OffsetFromBeginning(pair->second.key_offset);
                info->file->WriteKeyPair(stored_pair);
                // Offset to data
                info->file->OffsetFromBeginning(pair->second.data_offset);
                // Allocate space for data
                data->resize(pair->second.size);
                // Read the data
                info->file->ReadArray(const_cast<char*>(data->c_str()), (size_t)pair->second.size);
                // Finally
                info->file->Close();
                return true;
            }
            else
                return false;
        }
        bool Storage::Save(const DataKey& key, const std::string * data, StorageInfo& info)
        {
            if (data->empty())
            {
                LOG_ERROR("satellite imagery: curl returned empty data for key(%i,%i,%i)",
                              key.z(), key.x(), key.y());
                return true;
            }
            if (info.file->OpenForRead())
            {
                // We will protect all write operations to avoid data corruption
                // (application may crash due to external reason, a.e.)
                info.file->MarkOperationsBegin();
                // The main save logics is done here
                StorageFile::SaveResult result = info.file->SaveLogics(key, data, info.offsets, info.list);
                info.file->MarkOperationsEnd();
                // Finally
                info.file->Close();
                bool need_to_flush = false;
                if (result == StorageFile::kNeedToFlush)
                {
                    LOG_INFO("satellite imagery: storage flush due to reaching hard cap");
                    if (info.file->Flush())
                    {
                        need_to_flush = true;
                        info.offsets.clear();
                        info.list.clear();
                    }
                }
                {//---
                    std::lock_guard<std::mutex> guard(key_set_mutex_);
                    if (need_to_flush)
                        InitializeKeySet();
                    else
                        key_set_.insert(key);
                }//---
                return result == StorageFile::kSuccess;
            }
            else
                return false;
        }
        bool Storage::SaveMain(const DataKey& key, const std::string * data)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            return Save(key, data, main_info_);
        }
        bool Storage::SaveSeparate(const DataKey& key, const std::string * data, const std::string& name)
        {
            // Called from different thread than Load and Save, so we should protect shared data
            std::lock_guard<std::mutex> guard(mutex_);
            StorageInfoMap::iterator it = region_info_map_.find(name);
            if (it == region_info_map_.end())
            {
                LOG_ERROR("satellite imagery: did not find region storage with name '%s'", name.c_str());
                return false;
            }
            StorageInfo& info = it->second;
            return Save(key, data, info);
        }
        bool Storage::Region_Add(const RegionInfo& region_info)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            RegionMap::const_iterator it = region_map_.find(region_info.name);
            if (it != region_map_.end())
            {
                LOG_ERROR("satellite imagery: region '%s' already exists", region_info.name.c_str());
                return false; // already exists
            }

            // Generate a new filename
            std::string filename = GenerateRegionFileName();

            // Create a new storage file
            StorageFile * region_file = new StorageFile(filename, hash_value_, kMaxRegionFileSize);

            // Add a new object to map
            StoredRegionInfo& region = region_map_[region_info.name];
            region.info = region_info;
            region.status = RegionStatus::kDownloading;
            region.time = time(NULL); // use current time
            region.filename = filename;
            
            // Add file to region info map
            StorageInfo& storage_info = region_info_map_[region_info.name];
            storage_info.file = region_file;

            // Create file on disk
            region_file->Regenerate();

            // Write information into header file
            if (!regions_header_file_->AddRegion(region_map_))
                return false;
            
            return true;
        }
        bool Storage::Region_Rename(const std::string& old_name, const std::string& new_name)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            RegionMap::iterator it = region_map_.find(old_name);
            if (it == region_map_.end())
            {
                LOG_ERROR("satellite imagery: region '%s' hasn't been found", old_name.c_str());
                return false; // didn't find old one
            }

            // Change map object (name is also a map key, so we have to erase element)
            StoredRegionInfo region_copy = it->second;
            region_copy.info.name = new_name;
            region_map_.erase(it);
            region_map_.insert(std::make_pair(new_name, region_copy));

            // Also need to change files map
            StorageInfoMap::iterator itf = region_info_map_.find(old_name);
            assert(itf != region_info_map_.end());
            std::swap(region_info_map_[new_name], itf->second);
            region_info_map_.erase(itf);

            // Update header file information
            if (!regions_header_file_->Update(region_map_))
                return false;

            return true;
        }
        bool Storage::Region_Delete(const std::string& name)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            RegionMap::iterator it = region_map_.find(name);
            if (it == region_map_.end())
            {
                LOG_ERROR("satellite imagery: region '%s' hasn't been found", name.c_str());
                return false; // didn't find old one
            }

            // Remove region from map
            region_map_.erase(it);

            // Remove region file from list
            std::string filename;
            StorageInfoMap::iterator itf = region_info_map_.find(name);
            assert(itf != region_info_map_.end());
            StorageFile * file = itf->second.file;
            filename = file->GetFileName();
            delete file;
            region_info_map_.erase(itf);

            // Need to reinitialize key set
            {//---
                std::lock_guard<std::mutex> guard(key_set_mutex_);
                InitializeKeySet();
            }//---

            // Delete file from disk
            if (::remove(filename.c_str()) == 0)
            {
                LOG_INFO("satellite imagery: region '%s' has been deleted", name.c_str());
            }
            else
            {
                LOG_ERROR("satellite imagery: region '%s' deletion has been failed", name.c_str());
            }

            // Update header file information
            if (!regions_header_file_->RemoveRegion(region_map_))
                return false;

            return true;
        }
        bool Storage::Region_MarkStored(const std::string& name)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            RegionMap::iterator it = region_map_.find(name);
            if (it == region_map_.end())
            {
                LOG_ERROR("satellite imagery: region '%s' hasn't been found", name.c_str());
                return false;
            }
            // Update map data
            StoredRegionInfo& region_info = it->second;
            region_info.status = RegionStatus::kStored;

            // Update header file information
            if (!regions_header_file_->Update(region_map_))
                return false;

            return true;
        }
        bool Storage::Region_MarkInvalid(const std::string& name)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            RegionMap::iterator it = region_map_.find(name);
            if (it == region_map_.end())
            {
                LOG_ERROR("satellite imagery: region '%s' hasn't been found", name.c_str());
                return false;
            }
            // Update map data
            StoredRegionInfo& region_info = it->second;
            region_info.status = RegionStatus::kInvalid;

            // Update header file information
            if (!regions_header_file_->Update(region_map_))
                return false;

            return true;
        }
        bool Storage::GetRegionInfo(const std::string& name, StoredRegionInfo& info)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            RegionMap::const_iterator it = region_map_.find(name);
            if (it == region_map_.end())
            {
                return false;
            }
            const StoredRegionInfo& region_info = it->second;
            info = region_info;
            return true;
        }
        bool Storage::GetRegionsInfo(std::vector<StoredRegionInfo>& regions_info)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            for (RegionMap::const_iterator it = region_map_.begin(); it != region_map_.end(); ++it)
            {
                const StoredRegionInfo& info = it->second;
                regions_info.push_back(info);
            }
            return true;
        }
        BigFileSizeType Storage::GetRegionFileSize(const std::string& name)
        {
            std::lock_guard<std::mutex> guard(mutex_);
            RegionMap::const_iterator it = region_map_.find(name);
            if (it == region_map_.end())
            {
                return 0ULL;
            }
            const StoredRegionInfo& region_info = it->second;
            return static_cast<BigFileSizeType>(system::ObtainFileSize(region_info.filename.c_str()));
        }
        size_t Storage::GetKeyCount() const
        {
            volatile size_t size;
            {//---
                std::lock_guard<std::mutex> guard(const_cast<std::mutex&>(key_set_mutex_));
                size = key_set_.size();
            }//---
            return size;
        }
        BigFileSizeType Storage::GetEstimatedFileSize(unsigned int num_tiles, unsigned int data_size)
        {
            return StorageFile::GetEstimatedFileSize(num_tiles, data_size);
        }
        BigFileSizeType Storage::GetMaximumRegionFileSize()
        {
            return static_cast<BigFileSizeType>(kMaxRegionFileSize);
        }
        std::string Storage::AppendPath(const std::string& filename)
        {
            std::string path = path_;
            const char path_delimeter = sht::system::GetPathDelimeter();
            if (!path.empty() && path[path.length()-1] != path_delimeter)
                path += path_delimeter;
            return path + filename;
        }

    } // namespace satellite_imagery
} // namespace sht