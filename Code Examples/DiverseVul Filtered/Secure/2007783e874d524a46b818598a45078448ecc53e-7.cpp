void t_go_generator::generate_go_struct_initializer(ofstream& out,
                                                    t_struct* tstruct,
                                                    bool is_args_or_result) {
  out << publicize(type_name(tstruct), is_args_or_result) << "{";
  const vector<t_field*>& members = tstruct->get_members();
  for (vector<t_field*>::const_iterator m_iter = members.begin(); m_iter != members.end();
       ++m_iter) {
    bool pointer_field = is_pointer_field(*m_iter);
    string publicized_name;
    t_const_value* def_value;
    get_publicized_name_and_def_value(*m_iter, &publicized_name, &def_value);
    if (!pointer_field && def_value != NULL && !omit_initialization(*m_iter)) {
      out << endl << indent() << publicized_name << ": "
          << render_field_initial_value(*m_iter, (*m_iter)->get_name(), pointer_field) << ","
          << endl;
    }
  }

  out << "}" << endl;
}