void t_go_generator::get_publicized_name_and_def_value(t_field* tfield,
                                                       string* OUT_pub_name,
                                                       t_const_value** OUT_def_value) const {
  const string base_field_name = tfield->get_name();
  const string escaped_field_name = escape_string(base_field_name);
  *OUT_pub_name = publicize(escaped_field_name);
  *OUT_def_value = tfield->get_value();
}