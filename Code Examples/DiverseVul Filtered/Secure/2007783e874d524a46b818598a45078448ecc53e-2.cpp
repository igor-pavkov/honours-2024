static bool type_need_reference(t_type* type) {
  type = type->get_true_type();
  if (type->is_map() || type->is_set() || type->is_list() || type->is_struct()
      || type->is_xception() || (type->is_string() && ((t_base_type*)type)->is_binary())) {
    return false;
  }
  return true;
}