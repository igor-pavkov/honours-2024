bool t_go_generator::omit_initialization(t_field* tfield) {
  t_const_value* value = tfield->get_value();
  if (!value) {
    return true;
  }
  t_type* type = tfield->get_type()->get_true_type();
  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();

    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "";

    case t_base_type::TYPE_STRING:
      if (((t_base_type*)type)->is_binary()) {
        //[]byte are always inline
        return false;
      }
      // strings are pointers if has no default
      return value->get_string().empty();

    case t_base_type::TYPE_BOOL:
    case t_base_type::TYPE_I8:
    case t_base_type::TYPE_I16:
    case t_base_type::TYPE_I32:
    case t_base_type::TYPE_I64:
      return value->get_integer() == 0;
    case t_base_type::TYPE_DOUBLE:
      if (value->get_type() == t_const_value::CV_INTEGER) {
        return value->get_integer() == 0;
      } else {
        return value->get_double() == 0.;
      }
    }
  }
  return false;
}