// Galapix - an image viewer for large image collections
// Copyright (C) 2008-2019 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "archive_manager.hpp"

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string.h>

#include <logmich/log.hpp>

#include "util/filesystem.hpp"
#include "util/path.hpp"

#include "directory_extraction.hpp"
#include "incremental_extraction.hpp"
#include "rar_archive_loader.hpp"
#include "seven_zip_archive_loader.hpp"
#include "tar_archive_loader.hpp"
#include "zip_archive_loader.hpp"
#include "util/filesystem.hpp"

namespace arxp {

ArchiveManager::ArchiveManager() :
  ArchiveManager(std::filesystem::temp_directory_path().string())
{
}

ArchiveManager::ArchiveManager(std::filesystem::path const& tmpdir) :
  m_tmpdir(tmpdir),
  m_loader(),
  m_loader_by_file_exts(),
  m_loader_by_magic()
{
  add_loader(std::make_unique<RarArchiveLoader>());
  add_loader(std::make_unique<ZipArchiveLoader>());
  add_loader(std::make_unique<TarArchiveLoader>());
  add_loader(std::make_unique<SevenZipArchiveLoader>());
}

ArchiveManager::~ArchiveManager()
{
}

void
ArchiveManager::add_loader(std::unique_ptr<ArchiveLoader> loader)
{
  for(auto const& magic: loader->get_magics())
  {
    m_loader_by_magic[magic] = loader.get();
  }

  for(auto const& ext: loader->get_extensions())
  {
    m_loader_by_file_exts[ext] = loader.get();
  }

  m_loader.push_back(std::move(loader));
}

bool
ArchiveManager::is_archive(std::filesystem::path const& filename) const
{
  try
  {
    get_loader(filename);
    return true;
  }
  catch(...)
  {
    return false;
  }
}

bool
ArchiveManager::is_archive(std::span<uint8_t const> data) const
{
  log_error("not implemented");
  return false;
}

const ArchiveLoader&
ArchiveManager::get_loader(std::filesystem::path const& filename) const
{
  auto const* loader = find_loader_by_magic(filename);

  if (!loader)
  {
    loader = find_loader_by_filename(filename);
  }

  if (!loader)
  {
    throw std::runtime_error("failed to find loader for archive file: " + filename.string());
  }
  else
  {
    return *loader;
  }
}

const ArchiveLoader*
ArchiveManager::find_loader_by_filename(std::filesystem::path const& filename) const
{
  for(const auto& ext: m_loader_by_file_exts)
  {
    if (Filesystem::has_extension(filename, ext.first))
    {
      return ext.second;
    }
  }
  return nullptr;
}

const ArchiveLoader*
ArchiveManager::find_loader_by_magic(std::filesystem::path const& filename) const
{
  std::string start_of_file = Filesystem::get_magic(filename);

  // search for a loader that can handle the magic
  for(const auto& it: m_loader_by_magic)
  {
    if (start_of_file.starts_with(it.first))
    {
      return it.second;
    }
  }

  return nullptr;
}

std::vector<std::string>
ArchiveManager::get_filenames(std::filesystem::path const& zip_filename,
                              const ArchiveLoader** loader_out) const
{
  const auto& loader = get_loader(zip_filename);
  const auto& files = loader.get_filenames(zip_filename);
  if (loader_out)
  {
    *loader_out = &loader;
  }
  return files;
}

std::vector<uint8_t>
ArchiveManager::get_file(std::filesystem::path const& archive_filename, const std::string& filename) const
{
  const auto& loader = get_loader(archive_filename);
  return loader.get_file(archive_filename, filename);
}

std::shared_ptr<Extraction>
ArchiveManager::get_extraction(std::filesystem::path const& archive_filename) const
{
  const auto& loader = get_loader(archive_filename);
  if (loader.is_seekable(archive_filename))
  {
    return std::make_shared<IncrementalExtraction>(loader, archive_filename);
  }
  else
  {
    std::string tmpdir = create_extraction_directory();
    loader.extract(archive_filename, tmpdir);
    return std::make_shared<DirectoryExtraction>(tmpdir, loader.str());
  }
}

std::vector<uint8_t>
ArchiveManager::get_file(std::filesystem::path const& archive_filename, std::string const& type,
                         const std::string& filename) const
{
  auto it = std::find_if(m_loader.begin(), m_loader.end(),
                         [&type](std::unique_ptr<ArchiveLoader> const& loader) {
                           return loader->str() == type;
                         });

  if (it == m_loader.end()) {
    throw std::runtime_error("unknown archive type: " + type);
  } else {
    return (*it)->get_file(archive_filename, filename);
  }
}

std::string
ArchiveManager::create_extraction_directory() const
{
  std::filesystem::path directory = m_tmpdir / unique_path();
  log_info("creating directory: {}", directory.string());
  std::filesystem::create_directory(directory);
  return directory.string();
}

} // namespace arxp

/* EOF */
