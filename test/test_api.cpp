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

#include <gtest/gtest.h>

#include <binbro/binbro.h>
#include <fstream>

TEST(binbro, generic)
{
    incr4k::binbro::GroupedFiles same_files_groups;

    std::filesystem::path test_data_dir("../../../../../core/binbro/test/testdata");
    auto dir_to_check = std::filesystem::absolute(test_data_dir);
    incr4k::binbro b(dir_to_check);

    ASSERT_EQ(b.run(same_files_groups), incr4k::rc::ok);

    auto e = [dir_to_check](const char* path) { return dir_to_check / path; };

    incr4k::binbro::GroupedFiles expected{
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
    incr4k::binbro b(m_test_input);
    incr4k::binbro::GroupedFiles same_files_groups;

    ASSERT_EQ(b.run(same_files_groups), incr4k::rc::not_a_dir);
    ASSERT_TRUE(same_files_groups.empty());
}

TEST_F(wrong_input, file_as_dir)
{
    incr4k::binbro b(m_test_input);
    incr4k::binbro::GroupedFiles same_files_groups;

    std::ofstream file;
    file << "file";
    file.close();

    ASSERT_EQ(b.run(same_files_groups), incr4k::rc::not_a_dir);
    ASSERT_TRUE(same_files_groups.empty());
}

TEST_F(wrong_input, dir_as_symlink)
{
    incr4k::binbro::GroupedFiles same_files_groups;

    std::filesystem::create_directory(m_test_input);
    std::filesystem::create_directory_symlink(m_test_input, m_test_input_link);
    incr4k::binbro b(m_test_input_link);

    ASSERT_EQ(b.run(same_files_groups), incr4k::rc::not_a_dir);
    ASSERT_TRUE(same_files_groups.empty());
}

TEST(performace, DISABLED_huge_files)
{
    std::filesystem::path dir_to_test("D:\\Games");
    incr4k::binbro b(dir_to_test);
    incr4k::binbro::GroupedFiles same_files_groups;

    ASSERT_EQ(b.run(same_files_groups), incr4k::rc::ok);
    ASSERT_TRUE(same_files_groups.empty());
}
