inline bool str_has_cntrl(const std::string s) {
  return std::any_of(s.begin(), s.end(), ::iscntrl);
}