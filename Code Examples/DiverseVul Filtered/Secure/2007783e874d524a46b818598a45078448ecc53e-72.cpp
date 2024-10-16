void t_go_generator::generate_isset_helpers(ofstream& out,
                                            t_struct* tstruct,
                                            const string& tstruct_name,
                                            bool is_result) {
  (void)is_result;
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  const string escaped_tstruct_name(escape_string(tstruct->get_name()));

  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    const string field_name(publicize(escape_string((*f_iter)->get_name())));
    if ((*f_iter)->get_req() == t_field::T_OPTIONAL || is_pointer_field(*f_iter)) {
      out << indent() << "func (p *" << tstruct_name << ") IsSet" << field_name << "() bool {"
          << endl;
      indent_up();
      t_type* ttype = (*f_iter)->get_type()->get_true_type();
      bool is_byteslice = ttype->is_base_type() && ((t_base_type*)ttype)->is_binary();
      bool compare_to_nil_only = ttype->is_set() || ttype->is_list() || ttype->is_map()
                                 || (is_byteslice && !(*f_iter)->get_value());
      if (is_pointer_field(*f_iter) || compare_to_nil_only) {
        out << indent() << "return p." << field_name << " != nil" << endl;
      } else {
        string def_var_name = tstruct_name + "_" + field_name + "_DEFAULT";
        if (is_byteslice) {
          out << indent() << "return !bytes.Equal(p." << field_name << ", " << def_var_name << ")"
              << endl;
        } else {
          out << indent() << "return p." << field_name << " != " << def_var_name << endl;
        }
      }
      indent_down();
      out << indent() << "}" << endl << endl;
    }
  }
}