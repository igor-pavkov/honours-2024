void t_go_generator::generate_go_docstring(ofstream& out, t_doc* tdoc) {
  if (tdoc->has_doc()) {
    generate_docstring_comment(out, "", "//", tdoc->get_doc(), "");
  }
}