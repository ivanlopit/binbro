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

#pragma once

#include "binbro/rc.h"

#include <filesystem>
#include <string>

#include <map>

namespace incr4k
{
class binbro
{
    struct options
    {
        size_t thread_payload = 2 << 11;
        size_t use_hash_threashold = 2 << 3;
    };

    struct file_context
    {
        std::filesystem::path path;
        size_t size = 0;
        std::string checksum;
        rc status = rc::unknown;
    };

    struct file_content_less
    {
        bool operator()(const file_context& l, const file_context& r) const;
    };

    struct file_size_less
    {
        bool
        operator()(const file_context& l, const file_context& r) const
        {
            return l.size < r.size;
        }
    };

public:
    using GroupedFiles = std::vector<std::vector<std::filesystem::path>>;

    binbro(const std::filesystem::path& path, const options& opts = options())
        : m_path(path)
        , m_opts(opts)
    {
    }

    binbro(binbro&&) = delete;
    binbro(binbro&) = delete;

    binbro& operator=(binbro&&) = delete;
    binbro& operator=(binbro&) = delete;

    rc run(GroupedFiles& same_files_groups);

private:
    using Group = std::vector<file_context>;
    using GroupList = std::vector<Group>;
    using GroupedBySize = std::map<file_context, Group, file_size_less>;

    GroupedBySize group_by_size(const std::filesystem::path& path);
    GroupedFiles group_by_content(GroupedBySize& by_sizes);

    static GroupList hanle_size_group(GroupList gl, size_t use_hash_threashold);
    static void get_candidates_by_hash(GroupList& same, Group& group_to_handle);
    static void get_candidates_by_comparison(GroupList& same, Group& group_to_handle);

    options m_opts;
    std::filesystem::path m_path;
};

}  // namespace incr4k
