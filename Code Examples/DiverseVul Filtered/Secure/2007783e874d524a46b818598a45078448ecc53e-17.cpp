void t_go_generator::generate_const(t_const* tconst) {
  t_type* type = tconst->get_type();
  string name = publicize(tconst->get_name());
  t_const_value* value = tconst->get_value();

  if (type->is_base_type() || type->is_enum()) {
    indent(f_consts_) << "const " << name << " = " << render_const_value(type, value, name) << endl;
  } else {
    f_const_values_ << indent() << name << " = " << render_const_value(type, value, name) << endl
                    << endl;

    f_consts_ << indent() << "var " << name << " " << type_to_go_type(type) << endl;
  }
}