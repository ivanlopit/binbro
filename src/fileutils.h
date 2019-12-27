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

#include <string>
#include <filesystem>

#include "binbro/rc.h"

namespace incr4k
{
struct fileutils
{
    static rc calculate_md5(const std::filesystem::path& path,
                            std::string& checksum,
                            size_t block_size = 4096);

    static rc compare_files(const std::filesystem::path& l,
                            size_t l_size,
                            const std::filesystem::path& r,
                            size_t r_size,
                            size_t block_size = 4096);
};
}  // namespace incr4k
