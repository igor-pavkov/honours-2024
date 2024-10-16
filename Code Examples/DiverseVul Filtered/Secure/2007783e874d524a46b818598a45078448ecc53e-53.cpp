std::string t_go_generator::variable_name_to_go_name(const std::string& value) {
  if (value.size() <= 0) {
    return value;
  }

  std::string value2(value);
  std::transform(value2.begin(), value2.end(), value2.begin(), ::tolower);

  switch (value[0]) {
  case 'b':
  case 'B':
    if (value2 != "break") {
      return value;
    }

    break;

  case 'c':
  case 'C':
    if (value2 != "case" && value2 != "chan" && value2 != "const" && value2 != "continue") {
      return value;
    }

    break;

  case 'd':
  case 'D':
    if (value2 != "default" && value2 != "defer") {
      return value;
    }

    break;

  case 'e':
  case 'E':
    if (value2 != "else" && value2 != "error") {
      return value;
    }

    break;

  case 'f':
  case 'F':
    if (value2 != "fallthrough" && value2 != "for" && value2 != "func") {
      return value;
    }

    break;

  case 'g':
  case 'G':
    if (value2 != "go" && value2 != "goto") {
      return value;
    }

    break;

  case 'i':
  case 'I':
    if (value2 != "if" && value2 != "import" && value2 != "interface") {
      return value;
    }

    break;

  case 'm':
  case 'M':
    if (value2 != "map") {
      return value;
    }

    break;

  case 'p':
  case 'P':
    if (value2 != "package") {
      return value;
    }

    break;

  case 'r':
  case 'R':
    if (value2 != "range" && value2 != "return") {
      return value;
    }

    break;

  case 's':
  case 'S':
    if (value2 != "select" && value2 != "struct" && value2 != "switch") {
      return value;
    }

    break;

  case 't':
  case 'T':
    if (value2 != "type") {
      return value;
    }

    break;

  case 'v':
  case 'V':
    if (value2 != "var") {
      return value;
    }

    break;

  default:
    return value;
  }

  return value2 + "_a1";
}