#include <gtest/gtest.h>

#include "archive/archive_manager.hpp"
#include "archive/extraction.hpp"
#include "archive/util.hpp"

class ArchiveManagerTest : public ::testing::TestWithParam<std::string>
{
public:
  ArchiveManagerTest() :
    archive()
  {}

  ArchiveManager archive;
};

TEST_P(ArchiveManagerTest, is_archive)
{
  EXPECT_TRUE(archive.is_archive(GetParam()));
}

TEST_P(ArchiveManagerTest, get_filenames)
{
  EXPECT_EQ(archive.get_filenames(GetParam()), std::vector<std::string>({"text.txt"}));
}

TEST_P(ArchiveManagerTest, get_file)
{
  EXPECT_EQ(archive.get_file(GetParam(), "text.txt"),
            read_file("data/text.txt"));
}

TEST_P(ArchiveManagerTest, get_extraction)
{
  auto extraction = archive.get_extraction(GetParam());

  EXPECT_EQ(extraction->get_file("text.txt"), read_file("data/text.txt"));
  EXPECT_EQ(extraction->get_filenames(), std::vector<std::string>({"text.txt"}));
}

INSTANTIATE_TEST_CASE_P(ParamArchiveManagerTest, ArchiveManagerTest,
                        ::testing::Values("data/archive.tar",
                                          "data/archive.tar.gz",
                                          "data/archive.zip",
                                          "data/archive.rar"));

/* EOF */
