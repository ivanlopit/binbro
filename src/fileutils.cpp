/******************************************************************************

Copyright 2019 Lopit Ivan, lopit.i.i@gmail.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

******************************************************************************/

#include <filesystem>
#include <array>
#include <vector>

#include "fileutils.h"
#include "md5.h"

namespace incr4k
{
rc
fileutils::calculate_md5(const std::filesystem::path& path,
                         std::string& checksum,
                         size_t block_size)
{
    MD5_CTX ctx;
    MD5Init(ctx);

    auto handle = fopen(path.u8string().c_str(), "rb");

    if (!handle)
    {
        return rc::file_error;
    }
    std::vector<char> buffer(block_size, '\0');

    size_t readed = 0;
    while ((readed = fread(buffer.data(), 1, block_size, handle)) > 0)
    {
        MD5Update(ctx, (unsigned char*)buffer.data(), readed);
    }

    checksum = MD5Final(ctx);

    fclose(handle);

    return rc::ok;
}

rc
fileutils::compare_files(const std::filesystem::path& l,
                         size_t l_size,
                         const std::filesystem::path& r,
                         size_t r_size,
                         size_t block_size)
{
    // TODO, RAII for handles
    if (l_size != r_size)
    {
        return l_size < r_size ? rc::less : rc::greater;
    }

    auto lh = fopen(l.u8string().c_str(), "rb");
    if (!lh)
    {
        return rc::failed;
    }

    auto rh = fopen(r.u8string().c_str(), "rb");
    if (!rh)
    {
        fclose(lh);
        return rc::failed;
    }

    std::vector<char> buffer(block_size * 2, '\0');

    size_t lreaded = 0;
    size_t rreaded = 0;

    do
    {
        lreaded = fread(buffer.data(), 1, block_size, lh);
        rreaded = fread(buffer.data() + block_size, 1, block_size, rh);

        // expected that files with the same files return same fread object count
        if (lreaded != rreaded)
        {
            fclose(lh);
            fclose(rh);

            return rc::failed;
        }

        auto res = memcmp(buffer.data(), buffer.data() + block_size, lreaded);

        if (res != 0)
        {
            fclose(lh);
            fclose(rh);

            return res > 0 ? rc::less : rc::greater;
        }
    } while (lreaded > 0 && rreaded > 0);

    fclose(lh);
    fclose(rh);
    return rc::same;
}

}  // namespace incr4k
