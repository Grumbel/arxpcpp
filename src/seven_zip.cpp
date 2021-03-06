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

#include "seven_zip.hpp"

#include <stdexcept>

#include <exsp/exec.hpp>

namespace arxp {

namespace {

bool has_prefix(const std::string& lhs, const std::string& rhs)
{
  if (lhs.length() < rhs.length()) {
    return false;
  } else {
    return lhs.compare(0, rhs.length(), rhs) == 0;
  }
}

} // namespace

std::vector<std::string>
SevenZip::get_filenames(std::filesystem::path const& zip_filename)
{
  std::vector<std::string> lst;

  exsp::Exec zip("7zr");
  zip.arg("l").arg("-slt");
  zip.arg(zip_filename);

  if (zip.exec() != 0)
  {
    throw std::runtime_error("SevenZip::get_filenames(): " + std::string(zip.get_stderr().begin(), zip.get_stderr().end()));
  }
  else
  {
    auto const& stdout_lst = zip.get_stdout();
    auto line_start = stdout_lst.begin();
    bool parse_files = false;
    std::string file_start = "----------";
    for(auto i = stdout_lst.begin(); i != stdout_lst.end(); ++i)
    {
      if (*i == '\n')
      {
        std::string line(line_start, i);
        line_start = i + 1;

        if (!parse_files)
        {
          if (line == file_start)
          {
            parse_files = true;
          }
        }
        else
        {
          if (has_prefix(line, "Path = "))
          {
            lst.push_back(line.substr(7));
          }
        }
      }
    }
  }
  return lst;
}

std::vector<uint8_t>
SevenZip::get_file(std::filesystem::path const& zip_filename, const std::string& filename)
{
  exsp::Exec zip("7zr");
  zip.arg("x").arg("-so");
  zip.arg(zip_filename);
  zip.arg(filename);

  if (zip.exec() == 0)
  {
    return zip.move_stdout();
  }
  else
  {
    throw std::runtime_error(zip.str() + "\n" + std::string(zip.get_stderr().begin(), zip.get_stderr().end()));
  }
}

void
SevenZip::extract(std::filesystem::path const& archive, std::filesystem::path const& target_directory)
{
  exsp::Exec zip("7zr");
  zip.arg("x").arg("-o" + target_directory.string());
  zip.arg(archive);
  if (zip.exec() != 0)
  {
    throw std::runtime_error(zip.str() + "\n" + std::string(zip.get_stderr().begin(), zip.get_stderr().end()));
  }
}

} // namespace arxp

/* EOF */
