void t_go_generator::generate_go_struct_definition(ofstream& out,
                                                   t_struct* tstruct,
                                                   bool is_exception,
                                                   bool is_result,
                                                   bool is_args) {
  const vector<t_field*>& members = tstruct->get_members();
  const vector<t_field*>& sorted_members = tstruct->get_sorted_members();
  vector<t_field*>::const_iterator m_iter;

  std::string tstruct_name(publicize(tstruct->get_name(), is_args || is_result));
  generate_go_docstring(out, tstruct);
  out << indent() << "type " << tstruct_name << " struct {" << endl;
  /*
     Here we generate the structure specification for the fastbinary codec.
     These specifications have the following structure:
     thrift_spec -> tuple of item_spec
     item_spec -> nil | (tag, type_enum, name, spec_args, default)
     tag -> integer
     type_enum -> TType.I32 | TType.STRING | TType.STRUCT | ...
     name -> string_literal
     default -> nil  # Handled by __init__
     spec_args -> nil  # For simple types
                | (type_enum, spec_args)  # Value type for list/set
                | (type_enum, spec_args, type_enum, spec_args)
                  # Key and value for map
                | (class_name, spec_args_ptr) # For struct/exception
     class_name -> identifier  # Basically a pointer to the class
     spec_args_ptr -> expression  # just class_name.spec_args

     TODO(dreiss): Consider making this work for structs with negative tags.
  */
  // TODO(dreiss): Look into generating an empty tuple instead of nil
  // for structures with no members.
  // TODO(dreiss): Test encoding of structs where some inner structs
  // don't have thrift_spec.
  indent_up();

  int num_setable = 0;
  if (sorted_members.empty() || (sorted_members[0]->get_key() >= 0)) {
    int sorted_keys_pos = 0;

    for (m_iter = sorted_members.begin(); m_iter != sorted_members.end(); ++m_iter) {
      // Set field to optional if field is union, this is so we can get a
      // pointer to the field.
      if (tstruct->is_union())
        (*m_iter)->set_req(t_field::T_OPTIONAL);
      if (sorted_keys_pos != (*m_iter)->get_key()) {
        int first_unused = std::max(1, sorted_keys_pos++);
        while (sorted_keys_pos != (*m_iter)->get_key()) {
          ++sorted_keys_pos;
        }
        int last_unused = sorted_keys_pos - 1;
        if (first_unused < last_unused) {
          indent(out) << "// unused fields # " << first_unused << " to " << last_unused << endl;
        } else if (first_unused == last_unused) {
          indent(out) << "// unused field # " << first_unused << endl;
        }
      }

      t_type* fieldType = (*m_iter)->get_type();
      string goType = type_to_go_type_with_opt(fieldType, is_pointer_field(*m_iter));
      string gotag = "db:\"" + escape_string((*m_iter)->get_name())  + "\" ";
      if ((*m_iter)->get_req() == t_field::T_OPTIONAL) {
        gotag += "json:\"" + escape_string((*m_iter)->get_name()) + ",omitempty\"";
      } else {
        gotag += "json:\"" + escape_string((*m_iter)->get_name()) + "\"";
      }

      // Check for user override of db and json tags using "go.tag"
      std::map<string, string>::iterator it = (*m_iter)->annotations_.find("go.tag");
      if (it != (*m_iter)->annotations_.end()) {
        gotag = it->second;
      }
      indent(out) << publicize((*m_iter)->get_name()) << " " << goType << " `thrift:\""
                  << escape_string((*m_iter)->get_name()) << "," << sorted_keys_pos;
      if ((*m_iter)->get_req() == t_field::T_REQUIRED) {
        out << ",required";
      }

      out << "\" " << gotag << "`" << endl;
      sorted_keys_pos++;
    }
  } else {
    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      // This fills in default values, as opposed to nulls
      out << indent() << publicize((*m_iter)->get_name()) << " "
          << type_to_go_type((*m_iter)->get_type()) << endl;
    }
  }

  indent_down();
  out << indent() << "}" << endl << endl;
  out << indent() << "func New" << tstruct_name << "() *" << tstruct_name << " {" << endl;
  out << indent() << "  return &";
  generate_go_struct_initializer(out, tstruct, is_result || is_args);
  out << indent() << "}" << endl << endl;
  // Default values for optional fields
  for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
    string publicized_name;
    t_const_value* def_value;
    get_publicized_name_and_def_value(*m_iter, &publicized_name, &def_value);
    t_type* fieldType = (*m_iter)->get_type();
    string goType = type_to_go_type_with_opt(fieldType, false);
    string def_var_name = tstruct_name + "_" + publicized_name + "_DEFAULT";
    if ((*m_iter)->get_req() == t_field::T_OPTIONAL || is_pointer_field(*m_iter)) {
      out << indent() << "var " << def_var_name << " " << goType;
      if (def_value != NULL) {
        out << " = " << render_const_value(fieldType, def_value, (*m_iter)->get_name());
      }
      out << endl;
    }
    if (is_pointer_field(*m_iter)) {
      string goOptType = type_to_go_type_with_opt(fieldType, true);
      string maybepointer = goOptType != goType ? "*" : "";
      out << indent() << "func (p *" << tstruct_name << ") Get" << publicized_name << "() "
          << goType << " {" << endl;
      out << indent() << "  if !p.IsSet" << publicized_name << "() {" << endl;
      out << indent() << "    return " << def_var_name << endl;
      out << indent() << "  }" << endl;
      out << indent() << "return " << maybepointer << "p." << publicized_name << endl;
      out << indent() << "}" << endl;
      num_setable += 1;
    } else {
      out << endl;
      out << indent() << "func (p *" << tstruct_name << ") Get" << publicized_name << "() "
          << goType << " {" << endl;
      out << indent() << "  return p." << publicized_name << endl;
      out << indent() << "}" << endl;
    }
  }

  if (tstruct->is_union() && num_setable > 0) {
    generate_countsetfields_helper(out, tstruct, tstruct_name, is_result);
  }

  generate_isset_helpers(out, tstruct, tstruct_name, is_result);
  generate_go_struct_reader(out, tstruct, tstruct_name, is_result);
  generate_go_struct_writer(out, tstruct, tstruct_name, is_result, num_setable > 0);

  out << indent() << "func (p *" << tstruct_name << ") String() string {" << endl;
  out << indent() << "  if p == nil {" << endl;
  out << indent() << "    return \"<nil>\"" << endl;
  out << indent() << "  }" << endl;
  out << indent() << "  return fmt.Sprintf(\"" << escape_string(tstruct_name) << "(%+v)\", *p)"
      << endl;
  out << indent() << "}" << endl << endl;

  if (is_exception) {
    out << indent() << "func (p *" << tstruct_name << ") Error() string {" << endl;
    out << indent() << "  return p.String()" << endl;
    out << indent() << "}" << endl << endl;
  }
}