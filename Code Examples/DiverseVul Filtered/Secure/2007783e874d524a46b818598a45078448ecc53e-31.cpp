void t_go_generator::generate_go_struct_reader(ofstream& out,
                                               t_struct* tstruct,
                                               const string& tstruct_name,
                                               bool is_result) {
  (void)is_result;
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  string escaped_tstruct_name(escape_string(tstruct->get_name()));
  out << indent() << "func (p *" << tstruct_name << ") " << read_method_name_ << "(iprot thrift.TProtocol) error {"
      << endl;
  indent_up();
  out << indent() << "if _, err := iprot.ReadStructBegin(); err != nil {" << endl;
  out << indent() << "  return thrift.PrependError(fmt.Sprintf(\"%T read error: \", p), err)"
      << endl;
  out << indent() << "}" << endl << endl;

  // Required variables does not have IsSet functions, so we need tmp vars to check them.
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if ((*f_iter)->get_req() == t_field::T_REQUIRED) {
      const string field_name(publicize(escape_string((*f_iter)->get_name())));
      indent(out) << "var isset" << field_name << " bool = false;" << endl;
    }
  }
  out << endl;

  // Loop over reading in fields
  indent(out) << "for {" << endl;
  indent_up();
  // Read beginning field marker
  out << indent() << "_, fieldTypeId, fieldId, err := iprot.ReadFieldBegin()" << endl;
  out << indent() << "if err != nil {" << endl;
  out << indent() << "  return thrift.PrependError(fmt.Sprintf("
                     "\"%T field %d read error: \", p, fieldId), err)" << endl;
  out << indent() << "}" << endl;
  // Check for field STOP marker and break
  out << indent() << "if fieldTypeId == thrift.STOP { break; }" << endl;

  string thriftFieldTypeId;
  // Generate deserialization code for known cases
  int32_t field_id = -1;

  // Switch statement on the field we are reading, false if no fields present
  bool have_switch = !fields.empty();
  if (have_switch) {
    indent(out) << "switch fieldId {" << endl;
  }

  // All the fields we know
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    field_id = (*f_iter)->get_key();

    // if negative id, ensure we generate a valid method name
    string field_method_prefix("ReadField");
    int32_t field_method_suffix = field_id;

    if (field_method_suffix < 0) {
      field_method_prefix += "_";
      field_method_suffix *= -1;
    }

    out << indent() << "case " << field_id << ":" << endl;
    indent_up();
    thriftFieldTypeId = type_to_enum((*f_iter)->get_type());

    if (thriftFieldTypeId == "thrift.BINARY") {
      thriftFieldTypeId = "thrift.STRING";
    }

    out << indent() << "if err := p." << field_method_prefix << field_method_suffix << "(iprot); err != nil {"
        << endl;
    out << indent() << "  return err" << endl;
    out << indent() << "}" << endl;

    // Mark required field as read
    if ((*f_iter)->get_req() == t_field::T_REQUIRED) {
      const string field_name(publicize(escape_string((*f_iter)->get_name())));
      out << indent() << "isset" << field_name << " = true" << endl;
    }

    indent_down();
  }

  // Begin switch default case
  if (have_switch) {
    out << indent() << "default:" << endl;
    indent_up();
  }

  // Skip unknown fields in either case
  out << indent() << "if err := iprot.Skip(fieldTypeId); err != nil {" << endl;
  out << indent() << "  return err" << endl;
  out << indent() << "}" << endl;

  // End switch default case
  if (have_switch) {
    indent_down();
    out << indent() << "}" << endl;
  }

  // Read field end marker
  out << indent() << "if err := iprot.ReadFieldEnd(); err != nil {" << endl;
  out << indent() << "  return err" << endl;
  out << indent() << "}" << endl;
  indent_down();
  out << indent() << "}" << endl;
  out << indent() << "if err := iprot.ReadStructEnd(); err != nil {" << endl;
  out << indent() << "  return thrift.PrependError(fmt.Sprintf("
                     "\"%T read struct end error: \", p), err)" << endl;
  out << indent() << "}" << endl;

  // Return error if any required fields are missing.
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if ((*f_iter)->get_req() == t_field::T_REQUIRED) {
      const string field_name(publicize(escape_string((*f_iter)->get_name())));
      out << indent() << "if !isset" << field_name << "{" << endl;
      out << indent() << "  return thrift.NewTProtocolExceptionWithType(thrift.INVALID_DATA, "
                         "fmt.Errorf(\"Required field " << field_name << " is not set\"));" << endl;
      out << indent() << "}" << endl;
    }
  }

  out << indent() << "return nil" << endl;
  indent_down();
  out << indent() << "}" << endl << endl;

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    string field_type_name(publicize((*f_iter)->get_type()->get_name()));
    string field_name(publicize((*f_iter)->get_name()));
    string field_method_prefix("ReadField");
    int32_t field_id = (*f_iter)->get_key();
    int32_t field_method_suffix = field_id;

    if (field_method_suffix < 0) {
      field_method_prefix += "_";
      field_method_suffix *= -1;
    }

    out << indent() << "func (p *" << tstruct_name << ")  " << field_method_prefix << field_method_suffix
        << "(iprot thrift.TProtocol) error {" << endl;
    indent_up();
    generate_deserialize_field(out, *f_iter, false, "p.");
    indent_down();
    out << indent() << "  return nil" << endl;
    out << indent() << "}" << endl << endl;
  }
}