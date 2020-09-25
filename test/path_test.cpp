#include <set>

#include <gtest/gtest.h>

#include "util/path.hpp"

TEST(PathTest, unique_path)
{
  std::set<std::filesystem::path> collections;
  for(int i = 0; i < 1000; ++i) {
    std::filesystem::path p  = unique_path();
    EXPECT_EQ(p.string().size(), 16);
    EXPECT_TRUE(collections.find(p) == collections.end());
    collections.insert(p);
  }
}

/* EOF */
