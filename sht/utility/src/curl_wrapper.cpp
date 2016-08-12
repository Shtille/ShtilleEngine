#include "../include/curl_wrapper.h"

#include "../../system/include/stream/log_stream.h"

#include <curl/curl.h>

namespace sht {
    namespace utility {

        CurlWrapper::CurlWrapper()
        {
            curl_ = curl_easy_init();
            if (!curl_)
            {
				system::ErrorLogStream::GetInstance()->PrintString("failed to init libcurl");
            }
        }
        CurlWrapper::~CurlWrapper()
        {
            Cleanup();
        }
        bool CurlWrapper::Download(const char* url, void* userdata, CurlWriteFunction func)
        {
            if (curl_)
            {
                CURLcode res;
                curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1);
                curl_easy_setopt(curl_, CURLOPT_URL, url);
                curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
                curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
                curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
                curl_easy_setopt(curl_, CURLOPT_WRITEDATA, userdata);
                curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, func);
                res = curl_easy_perform(curl_);
                if (CURLE_OK == res)
                    return true;
                else
                {
                    system::ErrorLogStream::GetInstance()->PrintString("curl_easy_perform error %i", res);
                    return false;
                }
            }
            else
                return false;
        }
        void CurlWrapper::Cleanup()
        {
            if (curl_)
                curl_easy_cleanup(curl_);
        }

    } // namespace utility
} // namespace sht