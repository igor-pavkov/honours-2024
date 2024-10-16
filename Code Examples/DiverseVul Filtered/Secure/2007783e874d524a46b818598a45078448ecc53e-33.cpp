void t_go_generator::generate_typedef(t_typedef* ttypedef) {
  generate_go_docstring(f_types_, ttypedef);
  string new_type_name(publicize(ttypedef->get_symbolic()));
  string base_type(type_to_go_type(ttypedef->get_type()));

  if (base_type == new_type_name) {
    return;
  }

  f_types_ << "type " << new_type_name << " " << base_type << endl << endl;
  // Generate a convenience function that converts an instance of a type
  // (which may be a constant) into a pointer to an instance of a type.
  f_types_ << "func " << new_type_name << "Ptr(v " << new_type_name << ") *" << new_type_name
           << " { return &v }" << endl << endl;
}