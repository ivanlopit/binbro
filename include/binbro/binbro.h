#pragma once

#include "binbro/rc.h"

#include <filesystem>
#include <string>

namespace incr4k
{
class binbro
{
    struct file_context
    {
        std::filesystem::path path;
        std::string checksum;
        rc status;
    };

    struct file_less
    {
        bool operator()(const std::filesystem::path& l, const std::filesystem::path& r) const;
    };

public:
    using GroupedFiles = std::vector<std::vector<std::filesystem::path>>;

    rc run(const std::filesystem::path& path, GroupedFiles& same_files_groups);
};

}  // namespace incr4k
