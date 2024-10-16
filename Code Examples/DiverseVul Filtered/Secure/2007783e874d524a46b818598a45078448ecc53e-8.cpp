void t_go_generator::generate_go_struct_writer(ofstream& out,
                                               t_struct* tstruct,
                                               const string& tstruct_name,
                                               bool is_result,
                                               bool uses_countsetfields) {
  (void)is_result;
  string name(tstruct->get_name());
  const vector<t_field*>& fields = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator f_iter;
  indent(out) << "func (p *" << tstruct_name << ") " << write_method_name_ << "(oprot thrift.TProtocol) error {" << endl;
  indent_up();
  if (tstruct->is_union() && uses_countsetfields) {
    std::string tstruct_name(publicize(tstruct->get_name()));
    out << indent() << "if c := p.CountSetFields" << tstruct_name << "(); c != 1 {" << endl
        << indent()
        << "  return fmt.Errorf(\"%T write union: exactly one field must be set (%d set).\", p, c)"
        << endl << indent() << "}" << endl;
  }
  out << indent() << "if err := oprot.WriteStructBegin(\"" << name << "\"); err != nil {" << endl;
  out << indent() << "  return thrift.PrependError(fmt.Sprintf("
                     "\"%T write struct begin error: \", p), err) }" << endl;

  string field_name;
  string escape_field_name;
  // t_const_value* field_default_value;
  t_field::e_req field_required;
  int32_t field_id = -1;

  out << indent() << "if p != nil {" << endl;
  indent_up();

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    string field_method_prefix("writeField");
    field_name = (*f_iter)->get_name();
    escape_field_name = escape_string(field_name);
    field_id = (*f_iter)->get_key();
    int32_t field_method_suffix = field_id;

    if (field_method_suffix < 0) {
      field_method_prefix += "_";
      field_method_suffix *= -1;
    }

    out << indent() << "if err := p." << field_method_prefix << field_method_suffix
        << "(oprot); err != nil { return err }" << endl;
  }

  indent_down();
  out << indent() << "}" << endl;

  // Write the struct map
  out << indent() << "if err := oprot.WriteFieldStop(); err != nil {" << endl;
  out << indent() << "  return thrift.PrependError(\"write field stop error: \", err) }" << endl;
  out << indent() << "if err := oprot.WriteStructEnd(); err != nil {" << endl;
  out << indent() << "  return thrift.PrependError(\"write struct stop error: \", err) }" << endl;
  out << indent() << "return nil" << endl;
  indent_down();
  out << indent() << "}" << endl << endl;

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    string field_method_prefix("writeField");
    field_id = (*f_iter)->get_key();
    field_name = (*f_iter)->get_name();
    escape_field_name = escape_string(field_name);
    // field_default_value = (*f_iter)->get_value();
    field_required = (*f_iter)->get_req();
    int32_t field_method_suffix = field_id;

    if (field_method_suffix < 0) {
      field_method_prefix += "_";
      field_method_suffix *= -1;
    }

    out << indent() << "func (p *" << tstruct_name << ") " << field_method_prefix << field_method_suffix
        << "(oprot thrift.TProtocol) (err error) {" << endl;
    indent_up();

    if (field_required == t_field::T_OPTIONAL) {
      out << indent() << "if p.IsSet" << publicize(field_name) << "() {" << endl;
      indent_up();
    }

    out << indent() << "if err := oprot.WriteFieldBegin(\"" << escape_field_name << "\", "
        << type_to_enum((*f_iter)->get_type()) << ", " << field_id << "); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(fmt.Sprintf(\"%T write field begin error "
        << field_id << ":" << escape_field_name << ": \", p), err) }" << endl;

    // Write field contents
    generate_serialize_field(out, *f_iter, "p.");

    // Write field closer
    out << indent() << "if err := oprot.WriteFieldEnd(); err != nil {" << endl;
    out << indent() << "  return thrift.PrependError(fmt.Sprintf(\"%T write field end error "
        << field_id << ":" << escape_field_name << ": \", p), err) }" << endl;

    if (field_required == t_field::T_OPTIONAL) {
      indent_down();
      out << indent() << "}" << endl;
    }

    indent_down();
    out << indent() << "  return err" << endl;
    out << indent() << "}" << endl << endl;
  }
}