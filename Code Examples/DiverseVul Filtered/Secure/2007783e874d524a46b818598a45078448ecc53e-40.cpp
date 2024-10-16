std::string t_go_generator::privatize(const std::string& value) const {
  if (value.size() <= 0) {
    return value;
  }

  std::string value2(value);

  if (!islower(value2[0])) {
    value2[0] = tolower(value2[0]);
  }

  value2 = camelcase(value2);

  return value2;
}