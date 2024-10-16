void t_go_generator::generate_serialize_container(ofstream& out,
                                                  t_type* ttype,
                                                  bool pointer_field,
                                                  string prefix) {
  if (pointer_field) {
    prefix = "*" + prefix;
  }
  if (ttype->is_map()) {
    out << indent() << "if err := oprot.WriteMapBegin("
        << type_to_enum(((t_map*)ttype)->get_key_type()) << ", "
        << type_to_enum(((t_map*)ttype)->get_val_type()) << ", "
        << "len(" << prefix << ")); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error writing map begin: \", err)" << endl;
    out << indent() << "}" << endl;
  } else if (ttype->is_set()) {
    out << indent() << "if err := oprot.WriteSetBegin("
        << type_to_enum(((t_set*)ttype)->get_elem_type()) << ", "
        << "len(" << prefix << ")); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error writing set begin: \", err)" << endl;
    out << indent() << "}" << endl;
  } else if (ttype->is_list()) {
    out << indent() << "if err := oprot.WriteListBegin("
        << type_to_enum(((t_list*)ttype)->get_elem_type()) << ", "
        << "len(" << prefix << ")); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error writing list begin: \", err)" << endl;
    out << indent() << "}" << endl;
  } else {
    throw "compiler error: Invalid type in generate_serialize_container '" + ttype->get_name()
        + "' for prefix '" + prefix + "'";
  }

  if (ttype->is_map()) {
    t_map* tmap = (t_map*)ttype;
    out << indent() << "for k, v := range " << prefix << " {" << endl;
    indent_up();
    generate_serialize_map_element(out, tmap, "k", "v");
    indent_down();
    indent(out) << "}" << endl;
  } else if (ttype->is_set()) {
    t_set* tset = (t_set*)ttype;
    out << indent() << "for v, _ := range " << prefix << " {" << endl;
    indent_up();
    generate_serialize_set_element(out, tset, "v");
    indent_down();
    indent(out) << "}" << endl;
  } else if (ttype->is_list()) {
    t_list* tlist = (t_list*)ttype;
    out << indent() << "for _, v := range " << prefix << " {" << endl;

    indent_up();
    generate_serialize_list_element(out, tlist, "v");
    indent_down();
    indent(out) << "}" << endl;
  }

  if (ttype->is_map()) {
    out << indent() << "if err := oprot.WriteMapEnd(); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error writing map end: \", err)" << endl;
    out << indent() << "}" << endl;
  } else if (ttype->is_set()) {
    out << indent() << "if err := oprot.WriteSetEnd(); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error writing set end: \", err)" << endl;
    out << indent() << "}" << endl;
  } else if (ttype->is_list()) {
    out << indent() << "if err := oprot.WriteListEnd(); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(\"error writing list end: \", err)" << endl;
    out << indent() << "}" << endl;
  }
}