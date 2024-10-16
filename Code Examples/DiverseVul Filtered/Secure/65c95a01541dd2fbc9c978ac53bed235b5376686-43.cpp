static std::string make_relative_path(const std::string& path) {
  if (path.empty()) {
    return path;
  }

  // First get the path to a state where we don't have .. in the middle of it
  // etc. canonicalize handles Windows paths too.
  std::string canonical(FileUtil::canonicalize(path));

  // If we have a slash at the beginning, then just remove it and we are
  // relative. This check will hold because we have canonicalized the
  // path above to remove .. from the path, so we know we can be sure
  // we are at a good place for this check.
  if (FileUtil::isDirSeparator(canonical[0])) {
    return canonical.substr(1);
  }

  // If we get here, canonical looks something like:
  //   a/b/c

  // Search through the path and if we find a place where we have a slash
  // and a "." just before that slash, then cut the path off right there
  // and just take everything after the slash.
  std::string relative(canonical);
  int idx = canonical.length() - 1;
  while (1) {
    while (idx > 0 && !(FileUtil::isDirSeparator(canonical[idx]))) {
      idx--;
    }
    // If we ever get to idx == 0, then there were no other slashes to deal with
    if (idx == 0) {
      return canonical;
    }
    if (idx >= 1 && (canonical[idx - 1] == '.' || canonical[idx - 1] == ':')) {
      relative = canonical.substr(idx + 1);
      break;
    }
    idx--;
  }
  return relative;
}