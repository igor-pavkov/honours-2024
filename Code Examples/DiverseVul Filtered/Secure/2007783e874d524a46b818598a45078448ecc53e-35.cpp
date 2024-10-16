void t_go_generator::generate_go_docstring(ofstream& out, t_function* tfunction) {
  generate_go_docstring(out, tfunction, tfunction->get_arglist(), "Parameters");
}