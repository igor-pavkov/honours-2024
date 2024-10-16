static inline bool IsGlobbingPattern(const std::string& pattern) {
  return (pattern.find_first_of(kGlobbingChars) != std::string::npos);
}