string t_go_generator::type_to_go_key_type(t_type* type) {
  t_type* resolved_type = type;

  while (resolved_type->is_typedef()) {
    resolved_type = ((t_typedef*)resolved_type)->get_type()->get_true_type();
  }

  if (resolved_type->is_map() || resolved_type->is_list() || resolved_type->is_set()) {
    throw "Cannot produce a valid type for a Go map key: " + type_to_go_type(type) + " - aborting.";
  }

  if (resolved_type->is_string() && ((t_base_type*)resolved_type)->is_binary())
    return "string";

  return type_to_go_type(type);
}