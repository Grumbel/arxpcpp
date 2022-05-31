#include <cstdlib>
#include <ostream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <optional>

#include <fmt/format.h>

#include <arxp/archive_manager.hpp>

using namespace arxp;

namespace {

enum class Command {
  HELP,
  LIST,
  EXTRACT,
  // CHECKSUM,
};

struct Options
{
  Command command = Command::HELP;
  std::optional<std::filesystem::path> maybe_archive_file = std::nullopt;
  std::vector<std::filesystem::path> files = {};
};

Options parse_args(int argc, char** argv)
{
  if (argc < 2) {
    throw std::runtime_error(fmt::format("Usage: {} list FILE\n", argv[0]));
  }

  Options opts;

  // parse
  if (strcmp(argv[1], "list") == 0) {
    opts.command = Command::LIST;
  } else if (strcmp(argv[1], "help") == 0) {
    opts.command = Command::HELP;
  } else if (strcmp(argv[1], "extract") == 0) {
    opts.command = Command::EXTRACT;
  }

  for (int i = 2; i < argc; ++i)
  {
    if (opts.maybe_archive_file) {
      opts.files.emplace_back(argv[i]);
    } else {
      opts.maybe_archive_file = argv[i];
    }
  }

  return opts;
}

int cmd_help(Options const& opts)
{
  return EXIT_FAILURE;
}

int cmd_list(Options const& opts)
{
  if (!opts.maybe_archive_file) {
    throw std::runtime_error("no archive filename provided");
  }

  ArchiveManager arvmgr;

  auto files = arvmgr.get_filenames(*opts.maybe_archive_file);
  for (auto p : files) {
    std::cout << p << std::endl;
  }

  return EXIT_SUCCESS;
}

int cmd_extract(Options const& opts)
{
  throw std::runtime_error("not implemented");
}

int run(int argc, char** argv)
{
  Options opts = parse_args(argc, argv);

  switch (opts.command)
  {
    case Command::HELP:
      return cmd_help(opts);

    case Command::LIST:
      return cmd_list(opts);

    case Command::EXTRACT:
      return cmd_extract(opts);
  }

  return EXIT_FAILURE;
}

} // namespace

int main(int argc, char** argv) try
{
  return run(argc, argv);
}
catch (std::exception const& err) {
  std::cerr << "error: " << err.what() << std::endl;
}

/* EOF */
