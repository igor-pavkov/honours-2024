string t_go_generator::go_imports_begin(bool consts) {
  string extra;
  // If not writing constants, and there are enums, need extra imports.
  if (!consts && get_program()->get_enums().size() > 0) {
    extra =
      "\t\"database/sql/driver\"\n"
      "\t\"errors\"\n";
  }
  return string(
      "import (\n"
      "\t\"bytes\"\n"
      + extra +
      "\t\"fmt\"\n"
      "\t\"" + gen_thrift_import_ + "\"\n");
}