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

#include <iostream>
#include <binbro/binbro.h>

int
main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Wrong number of arguments!" << std::endl;
        return -1;
    }

    incr4k::binbro b(argv[1]);
    incr4k::binbro::GroupedFiles gf;
    auto rc = b.run(gf);
    if (rc != incr4k::rc::ok)
    {
        std::cout << "Application failed with error " << (int)rc;
        return -1;
    }
    for (const auto& g : gf)
    {
        std::cout << "Group of same files : " << std::endl;
        for (const auto& f : g)
        {
            std::cout << f << std::endl;
        }
        std::cout << std::endl;
    }
    return 0;
}
