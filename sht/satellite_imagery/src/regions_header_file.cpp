#include "regions_header_file.h"

#include "../../system/include/stream/log_stream.h"

#include <assert.h>

namespace sht {
    namespace satellite_imagery {

        static const unsigned int kFormatSignature = 1213353295;

        /*
        File version changes:
        Version 1.
            Original version.
        */
        static const unsigned char kVersion = 1;

        static const FileOffsetType kHeaderSize =
              sizeof(unsigned int)  // signature
            + sizeof(unsigned char) // version
            + sizeof(unsigned char) // corruption
            + sizeof(unsigned int)  // unique ID
            + sizeof(unsigned int); // number of regions
        static const FileOffsetType kCorruptionByteOffset = sizeof(unsigned int) + sizeof(unsigned char);
        static const FileOffsetType kUniqueIdOffset = kCorruptionByteOffset + sizeof(unsigned char);
        static const FileOffsetType kNumRegionsOffset = kUniqueIdOffset + sizeof(unsigned int);

        RegionsHeaderFile::RegionsHeaderFile(const std::string& filename)
            : File(filename)
            , unique_id_(0)
            , num_regions_(0)
        {
        }
        RegionsHeaderFile::~RegionsHeaderFile()
        {
        }
        unsigned int RegionsHeaderFile::GetUniqueID() const
        {
            return unique_id_;
        }
        bool RegionsHeaderFile::Flush()
        {
            if (OpenForWrite())
            {
                unique_id_ = 0;
                num_regions_ = 0;
                operation_successful_ = true;
                WriteHeader();
                Close();
                return success();
            }
            else
                return false;
        }
        void RegionsHeaderFile::MarkOperationsBegin()
        {
            FileOffsetType position = Tell();
            OffsetFromBeginning(kCorruptionByteOffset);
            WriteByte(1);
            OffsetFromBeginning(position);
        }
        void RegionsHeaderFile::MarkOperationsEnd()
        {
            FileOffsetType position = Tell();
            OffsetFromBeginning(kCorruptionByteOffset);
            WriteByte(0);
            OffsetFromBeginning(position);
        }
        void RegionsHeaderFile::WriteHeader()
        {
            WriteUint(kFormatSignature);
            WriteByte(kVersion);
            WriteByte(0); // corruption byte
            WriteUint(unique_id_);
            WriteUint(num_regions_);

#ifndef NDEBUG
            // Check that we have updated header size constant.
            FileOffsetType header_size = Tell();
            assert(header_size == kHeaderSize);
#endif
        }
        bool RegionsHeaderFile::ReadHeader()
        {
            // Read signature
            unsigned int signature;
            ReadUint(&signature);
            // Check signature
            if (signature != kFormatSignature)
            {
                LOG_ERROR("satellite imagery: wrong file signature, terminating");
                operation_successful_ = false;
                return false;
            }
            // Read version
            unsigned char version;
            ReadByte(&version);
            // Check version
            if (version < kVersion) // out of date
            {
                LOG_INFO("satellite imagery: format version %d is out of date. File will be cleaned.", version);
                Regenerate();
                return false;
            }
            // Read corruption byte
            unsigned char corrupted;
            ReadByte(&corrupted);
            // Check corruption
            if (corrupted)
            {
                LOG_INFO("satellite imagery: file is broken. It will be cleaned.");
                Regenerate();
                return false;
            }
            // Read unique ID
            ReadUint(&unique_id_);
            // Read regions count
            ReadUint(&num_regions_);
            return true;
        }
        void RegionsHeaderFile::WriteRegionInfo(const RegionInfo& region)
        {
            if (0 == fwrite(&region.upper_latitude, sizeof(region.upper_latitude), 1, file_))
            {
                LOG_ERROR("satellite imagery: fwrite failed");
                operation_successful_ = false;
            }
            if (0 == fwrite(&region.left_longitude, sizeof(region.left_longitude), 1, file_))
            {
                LOG_ERROR("satellite imagery: fwrite failed");
                operation_successful_ = false;
            }
            if (0 == fwrite(&region.lower_latitude, sizeof(region.lower_latitude), 1, file_))
            {
                LOG_ERROR("satellite imagery: fwrite failed");
                operation_successful_ = false;
            }
            if (0 == fwrite(&region.right_longitude, sizeof(region.right_longitude), 1, file_))
            {
                LOG_ERROR("satellite imagery: fwrite failed");
                operation_successful_ = false;
            }
            WriteString(region.name);
        }
        void RegionsHeaderFile::ReadRegionInfo(RegionInfo& region)
        {
            if (0 == fread(&region.upper_latitude, sizeof(region.upper_latitude), 1, file_))
            {
                LOG_ERROR("satellite imagery: fread failed");
                operation_successful_ = false;
            }
            if (0 == fread(&region.left_longitude, sizeof(region.left_longitude), 1, file_))
            {
                LOG_ERROR("satellite imagery: fread failed");
                operation_successful_ = false;
            }
            if (0 == fread(&region.lower_latitude, sizeof(region.lower_latitude), 1, file_))
            {
                LOG_ERROR("satellite imagery: fread failed");
                operation_successful_ = false;
            }
            if (0 == fread(&region.right_longitude, sizeof(region.right_longitude), 1, file_))
            {
                LOG_ERROR("satellite imagery: fread failed");
                operation_successful_ = false;
            }
            ReadString(&region.name);
        }
        void RegionsHeaderFile::WriteRegionStoredInfo(const StoredRegionInfo& region)
        {
            WriteRegionInfo(region.info);
            WriteUint(region.status);
            if (0 == fwrite(&region.time, sizeof(region.time), 1, file_))
            {
                LOG_ERROR("satellite imagery: fwrite failed");
                operation_successful_ = false;
            }
            WriteString(region.filename);
        }
        void RegionsHeaderFile::ReadRegionStoredInfo(StoredRegionInfo& region)
        {
            ReadRegionInfo(region.info);
            ReadUint(&region.status);
            if (0 == fread(&region.time, sizeof(region.time), 1, file_))
            {
                LOG_ERROR("satellite imagery: fwrite failed");
                operation_successful_ = false;
            }
            ReadString(&region.filename);
        }
        bool RegionsHeaderFile::AddRegion(const RegionMap& region_map)
        {
            ++unique_id_; // we need different ID for a new generated name
            ++num_regions_;
            return Update(region_map);
        }
        bool RegionsHeaderFile::RemoveRegion(const RegionMap& region_map)
        {
            --num_regions_;
            return Update(region_map);
        }
        bool RegionsHeaderFile::Update(const RegionMap& region_map)
        {
            assert(num_regions_ == static_cast<unsigned int>(region_map.size()));
            Close();
            if (OpenForWrite())
            {
                WriteHeader();
                // We will protect all write operations to avoid data corruption
                MarkOperationsBegin();

                for (RegionMap::const_iterator it = region_map.begin(); it != region_map.end(); ++it)
                {
                    const StoredRegionInfo& region_info = it->second;
                    WriteRegionStoredInfo(region_info);
                }

                MarkOperationsEnd();
                // Finally
                Close();

                return success();
            }
            else
                return false;
        }
        void RegionsHeaderFile::ReadAllRegions(RegionMap& region_map)
        {
            for (unsigned int i = 0; i < num_regions_; ++i)
            {
                StoredRegionInfo region_info;
                ReadRegionStoredInfo(region_info);
                region_map[region_info.info.name] = region_info;
            }
        }

    } // namespace satellite_imagery
} // namespace sht