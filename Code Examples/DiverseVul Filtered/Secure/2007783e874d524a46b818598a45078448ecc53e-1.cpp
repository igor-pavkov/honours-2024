std::string t_go_generator::new_prefix(const std::string& value) const {
  if (value.size() <= 0) {
    return value;
  }

  string::size_type dot_pos = value.rfind('.');
  if (dot_pos != string::npos) {
    return value.substr(0, dot_pos + 1) + "New" + publicize(value.substr(dot_pos + 1));
  }
  return "New" + publicize(value);
}