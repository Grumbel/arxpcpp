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

#ifndef HEADER_GALAPIX_ARCHIVE_TAR_HPP
#define HEADER_GALAPIX_ARCHIVE_TAR_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace arxp {

class Tar
{
public:
  static std::vector<std::string> get_filenames(std::filesystem::path const& tar_filename);
  static std::vector<uint8_t> get_file(std::filesystem::path const& tar_filename, const std::string& filename);

  /** Extract the content of \a archive to \a target_directory, \a
      target_directory must exist prior to calling this function */
  static void extract(std::filesystem::path const& tar_filename, std::filesystem::path const& target_directory);
};

#endif

} // namespace arxp

/* EOF */
