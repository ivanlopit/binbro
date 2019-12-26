
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

    incr4k::binbro b;

    incr4k::binbro::GroupedFiles gf;
    auto rc = b.run(argv[1], gf);
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
