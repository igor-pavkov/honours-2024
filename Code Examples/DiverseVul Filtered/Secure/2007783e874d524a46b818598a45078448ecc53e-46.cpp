void t_go_generator::generate_deserialize_struct(ofstream& out,
                                                 t_struct* tstruct,
                                                 bool pointer_field,
                                                 bool declare,
                                                 string prefix) {
  string eq(declare ? " := " : " = ");

  out << indent() << prefix << eq << (pointer_field ? "&" : "");
  generate_go_struct_initializer(out, tstruct);
  out << indent() << "if err := " << prefix << "." << read_method_name_ <<  "(iprot); err != nil {" << endl;
  out << indent() << "  return thrift.PrependError(fmt.Sprintf(\"%T error reading struct: \", "
      << prefix << "), err)" << endl;
  out << indent() << "}" << endl;
}