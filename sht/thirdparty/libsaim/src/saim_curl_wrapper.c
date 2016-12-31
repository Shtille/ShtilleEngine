/*************************************************************************
* libsaim 1.0
* ------------------------------------------------------------------------
*  Copyright (c) 2016 Vladimir Sviridov <v.shtille@gmail.com>
* 
*  This software is provided 'as-is', without any express or implied
*  warranty. In no event will the authors be held liable for any damages
*  arising from the use of this software.
* 
*  Permission is granted to anyone to use this software for any purpose,
*  including commercial applications, and to alter it and redistribute it
*  freely, subject to the following restrictions:
* 
*  1. The origin of this software must not be misrepresented; you must not
*     claim that you wrote the original software. If you use this software
*     in a product, an acknowledgment in the product documentation would
*     be appreciated but is not required.
* 
*  2. Altered source versions must be plainly marked as such, and must not
*     be misrepresented as being the original software.
* 
*  3. This notice may not be removed or altered from any source
*     distribution.
* 
**************************************************************************/

#include "saim_curl_wrapper.h"

#include "curl/curl.h"

#include <stdio.h>

bool saim_curl_wrapper__create(saim_curl_wrapper * curlw)
{
	curlw->curl = curl_easy_init();
	if (!curlw->curl)
	{
		fprintf(stderr, "saim: failed to init libcurl\n");
        return false;
	}
    return true;
}
void saim_curl_wrapper__destroy(saim_curl_wrapper * curlw)
{
	if (curlw->curl)
		curl_easy_cleanup(curlw->curl);
}
bool saim_curl_wrapper__download(saim_curl_wrapper * curlw, const char* url, void* userdata, CurlWriteFunction func)
{
	void* curl = curlw->curl;

	if (curl)
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, userdata);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, func);
        res = curl_easy_perform(curl);
        if (CURLE_OK == res)
            return true;
        else
        {
            fprintf(stderr, "saim: curl_easy_perform error %i\n", res);
            return false;
        }
    }
    else
        return false;
}