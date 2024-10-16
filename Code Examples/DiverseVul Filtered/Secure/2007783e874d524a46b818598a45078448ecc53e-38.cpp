string t_go_generator::type_to_go_type(t_type* type) {
  return type_to_go_type_with_opt(type, false);
}