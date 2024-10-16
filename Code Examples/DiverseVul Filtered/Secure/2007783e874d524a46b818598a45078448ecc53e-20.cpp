void t_go_generator::generate_go_docstring(ofstream& out, t_struct* tstruct) {
  generate_go_docstring(out, tstruct, tstruct, "Attributes");
}