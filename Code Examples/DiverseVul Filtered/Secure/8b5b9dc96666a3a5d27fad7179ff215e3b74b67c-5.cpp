static std::vector<std::string> AllDirectoryPrefixes(const std::string& d) {
  std::vector<std::string> dirs;
  const std::string patched = PatchPattern(d);
  StringPiece dir(patched);

  // If the pattern ends with a `/` (or `\\` on Windows), we need to strip it
  // otherwise we would have one additional matching step and the result set
  // would be empty.
  bool is_directory = d[d.size() - 1] == '/';
#ifdef PLATFORM_WINDOWS
  is_directory = is_directory || (d[d.size() - 1] == '\\');
#endif
  if (is_directory) {
    dir = io::Dirname(dir);
  }

  while (!dir.empty()) {
    dirs.emplace_back(dir);
    StringPiece new_dir(io::Dirname(dir));
    // io::Dirname("/") returns "/" so we need to break the loop.
    // On Windows, io::Dirname("C:\\") would return "C:\\", so we check for
    // identity of the result instead of checking for dir[0] == `/`.
    if (dir == new_dir) break;
    dir = new_dir;
  }

  // Order the array from parent to ancestor (reverse order).
  std::reverse(dirs.begin(), dirs.end());

  return dirs;
}