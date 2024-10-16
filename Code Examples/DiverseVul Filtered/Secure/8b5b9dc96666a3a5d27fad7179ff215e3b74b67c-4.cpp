static inline int GetFirstGlobbingEntry(const std::vector<std::string>& dirs) {
  int i = 0;
  for (const auto& d : dirs) {
    if (IsGlobbingPattern(d)) {
      break;
    }
    i++;
  }
  return i;
}