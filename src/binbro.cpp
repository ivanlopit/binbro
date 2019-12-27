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

#include "binbro/binbro.h"

#include "fileutils.h"
#include <iostream>
#include <future>
#include <map>
#include <vector>
#include <set>
#include <array>

namespace incr4k
{
rc
binbro::run(GroupedFiles& same_files_groups)
{
    if (std::filesystem::is_symlink(m_path) || !std::filesystem::is_directory(m_path))
    {
        return rc::not_a_dir;
    }

    auto by_size = group_by_size(m_path);

    same_files_groups = group_by_content(by_size);

    return rc::ok;
}

binbro::GroupedBySize
binbro::group_by_size(const std::filesystem::path& path)
{
    std::filesystem::recursive_directory_iterator itr(path);
    std::filesystem::recursive_directory_iterator end;

    GroupedBySize by_size;

    while (itr != end)
    {
        auto status = itr->status();

        if (status.type() == std::filesystem::file_type::regular &&
            (status.permissions() & std::filesystem::perms::owner_read) ==
                std::filesystem::perms::owner_read)
        {
            auto abs = std::filesystem::absolute(*itr);
            file_context fc{abs, std::filesystem::file_size(abs)};

            by_size[fc].emplace_back(std::move(fc));
        }
        else
        {
            std::cout << "Unable to process " << *itr << std::endl;
        }

        ++itr;
    }
    return by_size;
}

binbro::GroupedFiles
binbro::group_by_content(GroupedBySize& by_sizes)
{
    GroupedFiles same_files;

    size_t criteria = 0;
    GroupList group_to_handle;
    std::vector<std::future<GroupList>> async_results;

    for (auto& g : by_sizes)
    {
        if (g.second.size() > 1)
        {
            // TODO, no need to handle zero size, all zero size files are same
            criteria += g.second.size() * (g.second[0].size + 1);
            group_to_handle.push_back(std::move(g.second));

            if (criteria > m_opts.thread_payload)
            {
                async_results.emplace_back(std::async(std::launch::async, hanle_size_group,
                                                      std::move(group_to_handle),
                                                      m_opts.use_hash_threashold));

                group_to_handle = {};
                criteria = 0;
            }
        }
    }

    if (!group_to_handle.empty())
    {
        async_results.emplace_back(std::async(std::launch::async, hanle_size_group,
                                              std::move(group_to_handle),
                                              m_opts.use_hash_threashold));
    }

    for (auto& ar : async_results)
    {
        auto result_group = ar.get();

        for (auto& g : result_group)
        {
            if (g.size() > 1)
            {
                same_files.emplace_back();

                for (auto& f : g)
                {
                    same_files.back().emplace_back(std::move(f.path));
                }
            }
        }
    }

    return same_files;
}

void
binbro::get_candidates_by_hash(GroupList& same, Group& group_to_handle)
{
    std::map<std::string, Group> grouped_by_hash;

    for (auto& f : group_to_handle)
    {
        if (fileutils::calculate_md5(f.path, f.checksum) == rc::ok)
        {
            grouped_by_hash[f.checksum].emplace_back(std::move(f));
        }
    }

    for (auto& g : grouped_by_hash)
    {
        if (g.second.size() > 1)
        {
            get_candidates_by_comparison(same, g.second);
        }
    }
}

void
binbro::get_candidates_by_comparison(GroupList& same, Group& group_to_handle)
{
    std::map<file_context, Group, file_content_less> same_by_content;

    for (auto& f : group_to_handle)
    {
        same_by_content[f].emplace_back(std::move(f));
    }

    for (auto& g : same_by_content)
    {
        same.emplace_back(std::move(g.second));
    }
}

binbro::GroupList
binbro::hanle_size_group(GroupList gl, size_t use_hash_threashold)
{
    binbro::GroupList same;
    for (auto& g : gl)
    {
        if (g.size() > use_hash_threashold)
        {
            get_candidates_by_hash(same, g);
        }
        else
        {
            get_candidates_by_comparison(same, g);
        }
    }
    return same;
}

bool
binbro::file_content_less::operator()(const file_context& l, const file_context& r) const
{
    auto res = fileutils::compare_files(l.path, l.size, r.path, r.size);

    if (res == rc::failed)
    {
        // IDK, what to do here
        std::abort();
    }

    return res == rc::less;
}

}  // namespace incr4k
