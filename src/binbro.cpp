#include "binbro/binbro.h"

#include "hashapi.h"
#include <iostream>
#include <future>
#include <map>
#include <vector>
#include <set>
#include <array>

namespace incr4k
{
rc
binbro::run(const std::filesystem::path& path,
            std::vector<std::vector<std::filesystem::path>>& same_files_groups)
{
    if (!std::filesystem::is_directory(path))
    {
        return rc::not_a_dir;
    }

    std::filesystem::recursive_directory_iterator itr(path);
    std::filesystem::recursive_directory_iterator end;

    std::vector<std::future<file_context>> handles;

    while (itr != end)
    {
        if (itr->is_regular_file())
        {
            auto handle = std::async(
                std::launch::async,
                [](std::filesystem::path path) -> file_context {
                    std::string checksum;

                    auto rc = hash_api::calculate(path, checksum);
                    return file_context{std::move(path), std::move(checksum), rc};
                },
                std::filesystem::absolute(*itr));

            handles.push_back(std::move(handle));
        }

        ++itr;
    }

    std::map<std::string, std::vector<file_context>> files;

    for (auto& h : handles)
    {
        auto r = h.get();
        if (r.status == rc::ok)
        {
            files[r.checksum].push_back(std::move(r));
        }
    }

    for (auto& f : files)
    {
        if (f.second.size() > 1)
        {
            std::map<std::filesystem::path, std::vector<std::filesystem::path>, file_less> same;

            for (auto& f : f.second)
            {
                same[f.path].push_back(f.path);
            }

            for (auto& u : same)
            {
                same_files_groups.emplace_back();
                auto& last_group = same_files_groups.back();
                last_group.insert(last_group.end(), u.second.begin(), u.second.end());
            }
        }
    }

    return rc::ok;
}

bool
binbro::file_less::operator()(const std::filesystem::path& l, const std::filesystem::path& r) const
{
    auto res = hash_api::compare_files(l, r);

    if (res == rc::failed)
    {
        std::abort();
    }

    return res == rc::less;
}

}  // namespace incr4k
