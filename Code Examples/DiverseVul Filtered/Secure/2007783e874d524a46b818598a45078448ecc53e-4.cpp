string t_go_generator::go_imports_end() {
  return string(
      ")\n\n"
      "// (needed to ensure safety because of naive import list construction.)\n"
      "var _ = thrift.ZERO\n"
      "var _ = fmt.Printf\n"
      "var _ = bytes.Equal\n\n");
}