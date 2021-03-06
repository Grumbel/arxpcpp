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

#include "tar_archive_loader.hpp"

#include "archive_manager.hpp"
#include "directory_extraction.hpp"
#include "incremental_extraction.hpp"
#include "tar.hpp"

namespace arxp {

TarArchiveLoader::TarArchiveLoader()
{
}

std::vector<std::string>
TarArchiveLoader::get_magics() const
{
  return {};
}

std::vector<std::string>
TarArchiveLoader::get_extensions() const
{
  return { "tar", "tgz", "tar.gz", "tbz", "cbt", "tar.bz2", "taz", "tar.Z", "tlz", "tar.lz", "txz", "tar.xz" };
}

std::vector<std::string>
TarArchiveLoader::get_filenames(std::filesystem::path const& zip_filename) const
{
  return Tar::get_filenames(zip_filename);
}

std::vector<uint8_t>
TarArchiveLoader::get_file(std::filesystem::path const& zip_filename, const std::string& filename) const
{
  return Tar::get_file(zip_filename, filename);
}

void
TarArchiveLoader::extract(std::filesystem::path const& archive, std::filesystem::path const& target_directory) const
{
  Tar::extract(archive, target_directory);
}

} // namespace arxp

/* EOF */
