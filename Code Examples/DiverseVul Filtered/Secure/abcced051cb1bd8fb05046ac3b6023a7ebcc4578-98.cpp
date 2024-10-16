inline string PrintOneElement(const tstring& a, bool print_v2) {
  if (print_v2) {
    return "\"" + absl::Utf8SafeCEscape(a) + "\"";
  } else {
    return absl::Utf8SafeCEscape(a);
  }
}