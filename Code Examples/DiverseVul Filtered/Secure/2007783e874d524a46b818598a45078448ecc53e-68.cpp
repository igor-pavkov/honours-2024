bool t_go_generator::is_pointer_field(t_field* tfield, bool in_container_value) {
  (void)in_container_value;
  if (tfield->annotations_.count("cpp.ref") != 0) {
    return true;
  }
  t_type* type = tfield->get_type()->get_true_type();
  // Structs in containers are pointers
  if (type->is_struct() || type->is_xception()) {
    return true;
  }
  if (!(tfield->get_req() == t_field::T_OPTIONAL)) {
    return false;
  }

  bool has_default = tfield->get_value();
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
      return !has_default;

    case t_base_type::TYPE_BOOL:
    case t_base_type::TYPE_I8:
    case t_base_type::TYPE_I16:
    case t_base_type::TYPE_I32:
    case t_base_type::TYPE_I64:
    case t_base_type::TYPE_DOUBLE:
      return !has_default;
    }
  } else if (type->is_enum()) {
    return !has_default;
  } else if (type->is_struct() || type->is_xception()) {
    return true;
  } else if (type->is_map()) {
    return has_default;
  } else if (type->is_set()) {
    return has_default;
  } else if (type->is_list()) {
    return has_default;
  } else if (type->is_typedef()) {
    return has_default;
  }

  throw "INVALID TYPE IN type_to_go_type: " + type->get_name();
}