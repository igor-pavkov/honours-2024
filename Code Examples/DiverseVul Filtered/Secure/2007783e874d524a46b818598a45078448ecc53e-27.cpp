std::string t_go_generator::camelcase(const std::string& value) const {
  std::string value2(value);
  std::setlocale(LC_ALL, "C"); // set locale to classic

  // Fix common initialism in first word
  fix_common_initialism(value2, 0);

  // as long as we are changing things, let's change _ followed by lowercase to
  // capital and fix common initialisms
  for (std::string::size_type i = 1; i < value2.size() - 1; ++i) {
    if (value2[i] == '_') {
      if (islower(value2[i + 1])) {
        value2.replace(i, 2, 1, toupper(value2[i + 1]));
      }
      fix_common_initialism(value2, i);
    }
  }

  return value2;
}