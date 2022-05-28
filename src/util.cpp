/*
**  Galapix - an image viewer for large image collections
**  Copyright (C) 2018 Ingo Ruhnke <grumbel@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "util.hpp"

#include <errno.h>
#include <fstream>
#include <stdexcept>

#include <fmt/format.h>
#include <fmt/ostream.h>

namespace arxp {

void write_file(std::filesystem::path const& filename, std::span<uint8_t const> data)
{
  std::ofstream out(filename, std::ifstream::binary);
  if (!out) {
    throw std::system_error(errno, std::system_category(), "failed to open " + filename.string());
  }

  if (!out.write(reinterpret_cast<char const*>(data.data()), data.size())) {
    throw std::system_error(errno, std::system_category(), "failed to write " + filename.string());
  }
}

std::vector<uint8_t> read_file(std::filesystem::path const& filename)
{
  std::ifstream in(filename, std::ifstream::binary);
  if (!in) {
    throw std::system_error(errno, std::system_category(), "failed to open " + filename.string());
  }

  std::vector<uint8_t> data;
  uint8_t buffer[4096];
  while(!in.eof()) {
    if (!in.read(reinterpret_cast<char*>(buffer), 4096) && !in.eof()) {
      throw std::system_error(errno, std::system_category(), "failed to read " + filename.string());
    }
    data.reserve(data.size() + in.gcount());
    std::copy_n(buffer, in.gcount(), std::back_inserter(data));
  }

  return data;
}

} // namespace arxp

/* EOF */
