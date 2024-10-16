void t_go_generator::generate_deserialize_container(ofstream& out,
                                                    t_type* orig_type,
                                                    bool pointer_field,
                                                    bool declare,
                                                    string prefix) {
  t_type* ttype = get_true_type(orig_type);
  string eq(" = ");

  if (declare) {
    eq = " := ";
  }

  // Declare variables, read header
  if (ttype->is_map()) {
    out << indent() << "_, _, size, err := iprot.ReadMapBegin()" << endl;
    out << indent() << "if err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error reading map begin: \", err)" << endl;
    out << indent() << "}" << endl;
    out << indent() << "tMap := make(" << type_to_go_type(orig_type) << ", size)" << endl;
    out << indent() << prefix << eq << " " << (pointer_field ? "&" : "") << "tMap" << endl;
  } else if (ttype->is_set()) {
    t_set* t = (t_set*)ttype;
    out << indent() << "_, size, err := iprot.ReadSetBegin()" << endl;
    out << indent() << "if err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error reading set begin: \", err)" << endl;
    out << indent() << "}" << endl;
    out << indent() << "tSet := make(map["
        << type_to_go_key_type(t->get_elem_type()->get_true_type()) << "]struct{}, size)" << endl;
    out << indent() << prefix << eq << " " << (pointer_field ? "&" : "") << "tSet" << endl;
  } else if (ttype->is_list()) {
    out << indent() << "_, size, err := iprot.ReadListBegin()" << endl;
    out << indent() << "if err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error reading list begin: \", err)" << endl;
    out << indent() << "}" << endl;
    out << indent() << "tSlice := make(" << type_to_go_type(orig_type) << ", 0, size)" << endl;
    out << indent() << prefix << eq << " " << (pointer_field ? "&" : "") << "tSlice" << endl;
  } else {
    throw "INVALID TYPE IN generate_deserialize_container '" + ttype->get_name() + "' for prefix '"
        + prefix + "'";
  }

  // For loop iterates over elements
  out << indent() << "for i := 0; i < size; i ++ {" << endl;
  indent_up();

  if (pointer_field) {
    prefix = "(*" + prefix + ")";
  }
  if (ttype->is_map()) {
    generate_deserialize_map_element(out, (t_map*)ttype, declare, prefix);
  } else if (ttype->is_set()) {
    generate_deserialize_set_element(out, (t_set*)ttype, declare, prefix);
  } else if (ttype->is_list()) {
    generate_deserialize_list_element(out, (t_list*)ttype, declare, prefix);
  }

  indent_down();
  out << indent() << "}" << endl;

  // Read container end
  if (ttype->is_map()) {
    out << indent() << "if err := iprot.ReadMapEnd(); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error reading map end: \", err)" << endl;
    out << indent() << "}" << endl;
  } else if (ttype->is_set()) {
    out << indent() << "if err := iprot.ReadSetEnd(); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error reading set end: \", err)" << endl;
    out << indent() << "}" << endl;
  } else if (ttype->is_list()) {
    out << indent() << "if err := iprot.ReadListEnd(); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error reading list end: \", err)" << endl;
    out << indent() << "}" << endl;
  }
}