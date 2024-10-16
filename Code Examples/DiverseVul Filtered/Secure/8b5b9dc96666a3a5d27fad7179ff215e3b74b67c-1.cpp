static std::string PatchPattern(const std::string& pattern) {
  const std::string fixed_prefix =
      pattern.substr(0, pattern.find_first_of(kGlobbingChars));

  // Patching is needed when there is no directory part in `prefix`
  if (io::Dirname(fixed_prefix).empty()) {
    return io::JoinPath(".", pattern);
  }

  // No patching needed
  return pattern;
}