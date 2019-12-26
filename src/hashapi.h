#pragma once

#include <string>
#include <filesystem>

#include "binbro/rc.h"

namespace incr4k
{
struct hash_api
{
    static rc calculate(const std::filesystem::path& path,
                        std::string& checksum,
                        size_t block_size = 4096);

    static rc compare_files(const std::filesystem::path& l,
                            const std::filesystem::path& r,
                            size_t block_size = 4096);
};
}  // namespace incr4k
