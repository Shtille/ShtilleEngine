#include "file.h"

#include "../../system/include/stream/log_stream.h"

namespace sht {
    namespace satellite_imagery {

        File::File(const std::string& filename)
            : file_(NULL)
            , filename_(filename)
            , operation_successful_(false)
            , was_open_for_write_(false)
        {
        }
        File::~File()
        {
            if (file_)
            {
                fclose(file_);
                file_ = NULL;
            }
        }
        const std::string& File::GetFileName() const
        {
            return filename_;
        }
        bool File::success() const
        {
            return operation_successful_;
        }
        void File::WriteHeader()
        {
        }
        bool File::ReadHeader()
        {
            return true;
        }
        bool File::OpenForWrite()
        {
            if (file_) // opened
            {
                // Our main mode is read, so we gonna close the file
                fclose(file_);
                was_open_for_write_ = false;
            }
            if (file_ = fopen(GetFileName().c_str(), "wb"))
            {
                was_open_for_write_ = true;
                operation_successful_ = true;
                return true;
            }
            else
            {
                LOG_ERROR("Satellite Imagery: fopen failed");
                return false;
            }
        }
        bool File::OpenForRead()
        {
            if (!file_)
            {
                if (file_ = fopen(GetFileName().c_str(), "rb+")) // we will also use this option to update the file
                {
                    operation_successful_ = true;
                    return true;
                }
                else
                {
                    LOG_ERROR("Satellite Imagery: fopen failed: %s", GetFileName().c_str());
                    return false;
                }
            }
            else
            {
                // File is already open for reading
                Rewind();
                return true;
            }
        }
        void File::Rewind()
        {
            rewind(file_);
        }
        void File::Close()
        {
            if (file_)
            {
                if (was_open_for_write_)
                {
                    fclose(file_);
                    file_ = NULL;
                }
            }
            was_open_for_write_ = false;
        }
        void File::Regenerate()
        {
            Close();
            if (OpenForWrite())
            {
                WriteHeader();
            }
            else
            {
                LOG_ERROR("Satellite Imagery: file regeneration has failed.");
                operation_successful_ = false;
            }
        }
        void File::OffsetFromBeginning(long offset)
        {
            if (0 != fseek(file_, offset, SEEK_SET))
            {
                LOG_ERROR("Satellite Imagery: fseek failed");
                operation_successful_ = false;
            }
        }
        void File::OffsetFromCurrent(long offset)
        {
            if (0 != fseek(file_, offset, SEEK_CUR))
            {
                LOG_ERROR("Satellite Imagery: fseek failed");
                operation_successful_ = false;
            }
        }
        void File::OffsetFromEnd(long offset)
        {
            if (0 != fseek(file_, offset, SEEK_END))
            {
                LOG_ERROR("Satellite Imagery: fseek failed");
                operation_successful_ = false;
            }
        }
        FileOffsetType File::Tell()
        {
            return static_cast<FileOffsetType>(ftell(file_));
        }
        void File::WriteByte(unsigned char x)
        {
            if (0 == fwrite(&x, sizeof(x), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
        }
        void File::WriteInt(int x)
        {
            if (0 == fwrite(&x, sizeof(x), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
        }
        void File::WriteUint(unsigned int x)
        {
            if (0 == fwrite(&x, sizeof(x), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
        }
        void File::WriteArray(const void *data, size_t size)
        {
            if (0 == fwrite(data, size, 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
        }
        void File::WriteString(const std::string& str)
        {
            unsigned int length = static_cast<unsigned int>(str.size());
            if (0 == fwrite(&length, sizeof(length), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
            if (0 == fwrite(&str[0], length * sizeof(char), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fwrite failed");
                operation_successful_ = false;
            }
        }
        void File::ReadByte(unsigned char *x)
        {
            if (0 == fread(x, sizeof(unsigned char), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
        }
        void File::ReadInt(int *x)
        {
            if (0 == fread(x, sizeof(int), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
        }
        void File::ReadUint(unsigned int *x)
        {
            if (0 == fread(x, sizeof(unsigned int), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
        }
        void File::ReadArray(void *data, size_t size)
        {
            if (0 == fread(data, size, 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
        }
        void File::ReadString(std::string *str)
        {
            unsigned int length = 0;
            if (0 == fread(&length, sizeof(unsigned int), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
            str->resize(length);
            if (0 == fread(&(*str)[0], length * sizeof(char), 1, file_))
            {
                LOG_ERROR("Satellite Imagery: fread failed");
                operation_successful_ = false;
            }
        }

    } // namespace satellite_imagery
} // namespace sht