#include <gtest/gtest.h>

#include "archive/util.hpp"

using namespace arxp;

std::string const test_txt_str =
  "This program is free software: you can redistribute it and/or modify\n"
  "it under the terms of the GNU General Public License as published\n"
  "by the Free Software Foundation, either version 3 of the License,\n"
  "or (at your option) any later version.\n";

std::vector<uint8_t> const test_txt_data(reinterpret_cast<uint8_t const*>(test_txt_str.data()),
                                         reinterpret_cast<uint8_t const*>(test_txt_str.data() + test_txt_str.size()));

TEST(UtilTest, read_file)
{
  EXPECT_EQ(read_file("data/text.txt"), test_txt_data);

  EXPECT_ANY_THROW(read_file("does_not_exists.txt"));
}

TEST(UtilTest, write_file)
{
  std::filesystem::path tmpfile = std::filesystem::temp_directory_path() / "archcpp_util_test.txt";

  write_file(tmpfile, test_txt_data);

  EXPECT_EQ(read_file(tmpfile), test_txt_data);

  EXPECT_ANY_THROW(read_file("dir_does_not_exists/test.txt"));
}

/* EOF */
