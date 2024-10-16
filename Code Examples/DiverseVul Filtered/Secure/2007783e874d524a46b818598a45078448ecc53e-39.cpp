void t_go_generator::fix_common_initialism(std::string& value, int i) const {
  if (!ignore_initialisms_) {
    size_t wordLen = value.find('_', i);
    if (wordLen != std::string::npos) {
      wordLen -= i;
    }
    std::string word = value.substr(i, wordLen);
    std::transform(word.begin(), word.end(), word.begin(), ::toupper);
    if (commonInitialisms.find(word) != commonInitialisms.end()) {
      value.replace(i, word.length(), word);
    }
  }
}