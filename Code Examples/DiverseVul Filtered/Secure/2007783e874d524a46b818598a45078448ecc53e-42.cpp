void t_go_generator::generate_serialize_struct(ofstream& out, t_struct* tstruct, string prefix) {
  (void)tstruct;
  out << indent() << "if err := " << prefix << "." << write_method_name_ << "(oprot); err != nil {" << endl;
  out << indent() << "  return thrift.PrependError(fmt.Sprintf(\"%T error writing struct: \", "
      << prefix << "), err)" << endl;
  out << indent() << "}" << endl;
}