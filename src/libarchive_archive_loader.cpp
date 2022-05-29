#ifndef HEADER_ARXP_LIBARCHIVE_ARCHIVE_LOADER_HPP
#define HEADER_ARXP_LIBARCHIVE_ARCHIVE_LOADER_HPP

#include "archive_loader.hpp"

namespace arxp {

class LibarchiveArchiveLoader : public ArchiveLoader
{
public:
  LibarchiveArchiveLoader();

  std::vector<std::string> get_magics() const override;
  std::vector<std::string> get_extensions() const override;

  std::vector<std::string> get_filenames(std::filesystem::path const& archive_filename) const override;
  std::vector<uint8_t> get_file(std::filesystem::path const& archive_filename, const std::string& entry_filename) const override;
  void extract(std::filesystem::path const& archive_filename, std::filesystem::path const& target_directory) const override;
  bool is_seekable(std::filesystem::path const& archive_filename)  const override;

  std::string str() const { return "libarchive"; }

private:

private:
  LibarchiveArchiveLoader(const LibarchiveArchiveLoader&) = delete;
  LibarchiveArchiveLoader& operator=(const LibarchiveArchiveLoader&) = delete;
};

} // namespace arxp

#endif

/* EOF */
