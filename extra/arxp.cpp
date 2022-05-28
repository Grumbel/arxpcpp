#include <stdlib.h>
#include <iostream>

#include <fmt/format.h>

#include <arxp/archive_manager.hpp>

using namespace arxp;

int main(int argc, char** argv)
{
  if (argc < 2) {
    std::cerr << fmt::format("Usage: {} list FILE\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if (strcmp(argv[1], "list") == 0) {
    ArchiveManager archive;

    for (int i = 2; i < argc; ++i) {
      try {
      auto files = archive.get_filenames(argv[i]);
      for (auto p : files) {
        std::cout << p << std::endl;
      }
      } catch (std::exception const& err) {
        std::cerr << fmt::format("{}: {}\n", argv[i], err.what());
      }
    }
  }

  return 0;
}

/* EOF */
