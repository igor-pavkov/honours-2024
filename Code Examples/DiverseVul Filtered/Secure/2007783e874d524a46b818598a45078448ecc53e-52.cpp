void t_go_generator::close_generator() {
  f_const_values_ << "}" << endl << endl;
  f_consts_ << f_const_values_.str();

  // Close types and constants files
  f_consts_.close();
  f_types_.close();
  format_go_output(f_types_name_);
  format_go_output(f_consts_name_);
}