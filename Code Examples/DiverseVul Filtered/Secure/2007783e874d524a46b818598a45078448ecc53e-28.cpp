string t_go_generator::type_name(t_type* ttype) {
  string module( module_name(ttype));
  if( ! module.empty()) {
    return module + "." + ttype->get_name();
  }

  return ttype->get_name();
}