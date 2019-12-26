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

class wrong_input : public ::testing::Test
{
protected:
    void
    SetUp()
    {
        std::filesystem::remove(m_test_input);
        std::filesystem::remove(m_test_input_link);
    }
    std::filesystem::path m_test_input = "broken";
    std::filesystem::path m_test_input_link = "broken_link";
};

TEST_F(wrong_input, not_exists)
{
    incr4k::binbro b;
    std::vector<std::vector<std::filesystem::path>> same_files_groups;

    ASSERT_EQ(b.run(m_test_input, same_files_groups), incr4k::rc::not_a_dir);
    ASSERT_TRUE(same_files_groups.empty());
}

TEST_F(wrong_input, file_as_dir)
{
    incr4k::binbro b;
    std::vector<std::vector<std::filesystem::path>> same_files_groups;

    std::ofstream file;
    file << "file";
    file.close();

    ASSERT_EQ(b.run(m_test_input, same_files_groups), incr4k::rc::not_a_dir);
    ASSERT_TRUE(same_files_groups.empty());
}

TEST_F(wrong_input, dir_as_symlink)
{
    incr4k::binbro b;
    std::vector<std::vector<std::filesystem::path>> same_files_groups;

    std::filesystem::create_directory(m_test_input);

    std::filesystem::create_directory_symlink(m_test_input, m_test_input_link);

    ASSERT_EQ(b.run(m_test_input_link, same_files_groups), incr4k::rc::not_a_dir);
    ASSERT_TRUE(same_files_groups.empty());
}
