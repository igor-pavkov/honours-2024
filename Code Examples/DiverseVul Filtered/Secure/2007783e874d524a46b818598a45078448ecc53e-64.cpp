void t_go_generator::generate_countsetfields_helper(ofstream& out,
                                                    t_struct* tstruct,
                                                    const string& tstruct_name,
                                                    bool is_result) {
  (void)is_result;
  const vector<t_field*>& fields = tstruct->get_members();
  vector<t_field*>::const_iterator f_iter;
  const string escaped_tstruct_name(escape_string(tstruct->get_name()));

  out << indent() << "func (p *" << tstruct_name << ") CountSetFields" << tstruct_name << "() int {"
      << endl;
  indent_up();
  out << indent() << "count := 0" << endl;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    if ((*f_iter)->get_req() == t_field::T_REQUIRED)
      continue;

    if (!is_pointer_field(*f_iter))
      continue;

    const string field_name(publicize(escape_string((*f_iter)->get_name())));

    out << indent() << "if (p.IsSet" << field_name << "()) {" << endl;
    indent_up();
    out << indent() << "count++" << endl;
    indent_down();
    out << indent() << "}" << endl;
  }

  out << indent() << "return count" << endl << endl;
  indent_down();
  out << indent() << "}" << endl << endl;
}