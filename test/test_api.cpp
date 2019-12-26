#include <gtest/gtest.h>

#include <binbro/binbro.h>
#include <fstream>

TEST(binbro, generic)
{
    incr4k::binbro b;
    std::vector<std::vector<std::filesystem::path>> same_files_groups;

    std::filesystem::path test_data_dir("../../../../../core/binbro/test/testdata");
    auto dir_to_check = std::filesystem::absolute(test_data_dir);

    ASSERT_EQ(b.run(dir_to_check, same_files_groups), incr4k::rc::ok);

    auto e = [dir_to_check](const char* path) { return dir_to_check / path; };

    std::vector<std::vector<std::filesystem::path>> expected{
        {e("level0_0\\level1_0\\same.txt"), e("level0_1\\same.txt"), e("same.txt")},
        {e("collision_a"), e("level0_0\\collision_a")},
        {e("level0_0\\collision_b"), e("level0_1\\collision_b")}};

    ASSERT_EQ(same_files_groups, expected);
}

TEST(binbro, wrong_input)
{
    incr4k::binbro b;

    std::filesystem::path test_input("broken");
    std::filesystem::remove(test_input);
    std::vector<std::vector<std::filesystem::path>> same_files_groups;

    ASSERT_EQ(b.run(test_input, same_files_groups), incr4k::rc::not_a_dir);

    {
        std::ofstream file(test_input);
        file << "file";
    }

    ASSERT_EQ(b.run(test_input, same_files_groups), incr4k::rc::not_a_dir);
}
