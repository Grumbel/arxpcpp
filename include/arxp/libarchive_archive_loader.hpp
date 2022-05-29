#include "libarchive_archive_loader.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <fmt/format.hpp>
#include <logmich/logmich.hpp>

namespace arxp {

class ArchiveHandle
{
public:
  ArchiveHandle(std::string const& filename) :
    m_handle()
  {
    m_handle = archive_read_new();
    archive_read_support_filter_all(m_handle);
    archive_read_support_format_all(m_handle);

    open(filename);
  }

  ~ArchiveHandle()
  {
    if (m_handle != nullptr)
    {
      int const ret = archive_read_free(m_handle);
      if (ret != ARCHIVE_OK) {
        log_error("archive_read_free(): {}", archive_error_string(arv));
      }
    }
  }

  void open(std::string const& filename)
  {
    int const ret = archive_read_open_filename(m_handle, filename.c_str(), 10240);
    if (ret != ARCHIVE_OK)
    {
      throw std::runtime_error(fmt::format("archive_read_open_filename(): {}", archive_error_string(arv)));
    }
  }

  void close()
  {
    int const ret = archive_read_free(m_handle);
    m_handle = nullptr;

    if (ret != ARCHIVE_OK) {
      throw std::runtime_error(fmt::format("archive_read_free(): {}", archive_error_string(arv)));
    }
  }

  std::vector<std::string> get_filenames() const
  {
    std::vector<std::string> result;

    struct archive_entry* entry;
    while (archive_read_next_header(m_handle, &entry) == ARCHIVE_OK)
    {
      int64_t const bytes = archive_entry_size(entry);
      char const* pathname = archive_entry_pathname(entry);
      int const filetype = archive_entry_filetype(entry);

      entries.emplace_back(pathname);

      log_debug("{} {} ({} bytes)", ((filetype & AE_IFDIR) ? "[DIR]" : "[FILE]"), pathname, bytes);

      int ret = archive_read_data_skip(arv);
      if (ret != ARCHIVE_OK)
      {
        throw std::runtime_error(fmt::format("archive_read_data_skip(): {}", archive_error_string(arv)));
      }
    }

    return result;
  }

  std::vector<uint8_t> get_file(std::filesystem::path const& archive_filename, const std::string& entry_filename) const
  {
    struct archive_entry* entry;
    while (archive_read_next_header(m_handle, &entry) == ARCHIVE_OK)
    {
      char const* pathname = archive_entry_pathname(entry);
      int64_t const bytes = archive_entry_size(entry);
      int const filetype = archive_entry_filetype(entry);

      if (entry_filename == pathname) {
        std::vector<uint8_t> content(bytes);
        int ret = archive_read_data(arv, content.data(), bytes);
        if (ret != ARCHIVE_OK) {
          throw std::runtime_error(fmt::format("archive_read_data(): {}", archive_error_string(arv)));
        }
        return content;
      }
    }
    throw std::runtime_error(fmt::format("{}: entry not found", entry_filename));
  }

  void extract(std::filesystem::path const& target_directory) const
  {
    struct archive_entry* entry;
    while (archive_read_next_header(m_handle, &entry) == ARCHIVE_OK)
    {
      char const* pathname = archive_entry_pathname(entry);
      int64_t const bytes = archive_entry_size(entry);
      int const filetype = archive_entry_filetype(entry);

      // FIXME: make directory hierachy

      std::vector<uint8_t> buffer(bytes);
      int ret = archive_read_data(arv, content.data(), bytes);
      if (ret != ARCHIVE_OK) {
        throw std::runtime_error(fmt::format("archive_read_data(): {}", archive_error_string(arv)));
      }

      // FIXME: write content to file

      return content;
    }
  }

private:
  archive* m_handle;

public:
  ArchiveHandle(const ArchiveHandle&) = delete;
  ArchiveHandle& operator=(const ArchiveHandle&) = delete;
};

LibarchiveArchiveLoader::LibarchiveArchiveLoader()
{
}

std::vector<std::string>
LibarchiveArchiveLoader::get_magics() const
{
  return {};
}

std::vector<std::string>
LibarchiveArchiveLoader::get_extensions() const
{
  return { "zip", "rar", "7z" };
}

std::vector<std::string>
LibarchiveArchiveLoader::get_filenames(std::filesystem::path const& archive_filename) const
{
  ArchiveHandle arv(archive_filename);
  return arv.get_filenames();
}

std::vector<uint8_t>
LibarchiveArchiveLoader::get_file(std::filesystem::path const& archive_filename, const std::string& entry_filename) const
{
  ArchiveHandle arv(archive_filename);
  return arv.get_file(entry_filename);
}

void
LibarchiveArchiveLoader::extract(std::filesystem::path const& archive, std::filesystem::path const& target_directory) const
{
  ArchiveHandle arv(archive_filename);
  return arv.extract(target_directory);
}

bool
LibarchiveArchiveLoader::is_seekable(std::filesystem::path const& archive)  const
{
  return false;
}

} // namespace arxp

/* EOF */
