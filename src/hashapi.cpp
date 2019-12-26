#include <filesystem>
#include <array>
#include <vector>

#include "hashapi.h"
#include "md5.h"

namespace incr4k
{
rc
hash_api::calculate(const std::filesystem::path& path, std::string& checksum, size_t block_size)
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

    return rc::ok;
}

rc
hash_api::compare_files(const std::filesystem::path& l,
                        const std::filesystem::path& r,
                        size_t block_size)
{
    std::error_code ec;
    size_t l_size = std::filesystem::file_size(l);
    size_t r_size = std::filesystem::file_size(r);

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
            return rc::failed;
        }

        auto res = memcmp(buffer.data(), buffer.data() + block_size, lreaded);

        if (res != 0)
        {
            return res > 0 ? rc::less : rc::greater;
        }
    } while (lreaded > 0 && rreaded > 0);

    return rc::same;
}

}  // namespace incr4k
