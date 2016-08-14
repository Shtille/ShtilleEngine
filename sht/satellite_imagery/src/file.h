#pragma once
#ifndef __SHT_SATELLITE_IMAGERY_FILE_H__
#define __SHT_SATELLITE_IMAGERY_FILE_H__

#include "../include/defines.h"

#include "../../common/non_copyable.h"

#include <cstddef>
#include <stdio.h>
#include <string>

namespace sht {
    namespace satellite_imagery {

        //! Base file class
        class File : public NonCopyable {
        public:
            File(const std::string& filename);
            virtual ~File();

            //! Returns full path to file
            const std::string& GetFileName() const;

            bool success() const;

            virtual void WriteHeader();
            virtual bool ReadHeader();

        protected:
            bool OpenForWrite();
            bool OpenForRead();
            void Rewind();
            void Close();
            void Regenerate();

            // Offset operations
            void OffsetFromBeginning(long offset);
            void OffsetFromCurrent(long offset);
            void OffsetFromEnd(long offset);
            FileOffsetType Tell();

            // Write operations
            void WriteByte(unsigned char x);
            void WriteInt(int x);
            void WriteUint(unsigned int x);
            void WriteArray(const void *data, size_t size);
            void WriteString(const std::string& str);

            // Read operations
            void ReadByte(unsigned char *x);
            void ReadInt(int *x);
            void ReadUint(unsigned int *x);
            void ReadArray(void *data, size_t size);
            void ReadString(std::string *str);

            FILE * file_;
            std::string filename_;
            bool operation_successful_;
            bool was_open_for_write_;
        };

    } // namespace satellite_imagery
} // namespace sht

#endif