#pragma once
#ifndef __SHT_UTILITY_CURL_WRAPPER_H__
#define __SHT_UTILITY_CURL_WRAPPER_H__

#include <cstddef>

struct Curl_easy;

namespace sht {
    namespace utility {

        /*! Curl write function declaration
        @param buffer
        @param size
        @param nmemb
        @param userdata
        */
        typedef size_t (*CurlWriteFunction)(void*, size_t, size_t, void*);

        //! Curl wrapper class
        class CurlWrapper final {
        public:
            CurlWrapper();
            ~CurlWrapper();

            bool Download(const char* url, void* userdata, CurlWriteFunction func);

        private:
            CurlWrapper(const CurlWrapper&) = delete;
            CurlWrapper& operator =(const CurlWrapper&) = delete;

            void Cleanup();

            Curl_easy* curl_; //!< CURL handle
        };

    } // namespace utility
} // namespace sht

#endif