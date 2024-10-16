std::string t_go_generator::publicize(const std::string& value, bool is_args_or_result) const {
  if (value.size() <= 0) {
    return value;
  }

  std::string value2(value), prefix;

  string::size_type dot_pos = value.rfind('.');
  if (dot_pos != string::npos) {
    prefix = value.substr(0, dot_pos + 1) + prefix;
    value2 = value.substr(dot_pos + 1);
  }

  if (!isupper(value2[0])) {
    value2[0] = toupper(value2[0]);
  }

  value2 = camelcase(value2);

  // final length before further checks, the string may become longer
  size_t len_before = value2.length();

  // IDL identifiers may start with "New" which interferes with the CTOR pattern
  // Adding an extra underscore to all those identifiers solves this
  if ((len_before >= 3) && (value2.substr(0, 3) == "New")) {
    value2 += '_';
  }

  // IDL identifiers may end with "Args"/"Result" which interferes with the implicit service
  // function structs
  // Adding another extra underscore to all those identifiers solves this
  // Suppress this check for the actual helper struct names
  if (!is_args_or_result) {
    bool ends_with_args = (len_before >= 4) && (value2.substr(len_before - 4, 4) == "Args");
    bool ends_with_rslt = (len_before >= 6) && (value2.substr(len_before - 6, 6) == "Result");
    if (ends_with_args || ends_with_rslt) {
      value2 += '_';
    }
  }

  // Avoid naming collisions with other services
  if (is_args_or_result) {
    prefix += publicize(service_name_);
  }

  return prefix + value2;
}