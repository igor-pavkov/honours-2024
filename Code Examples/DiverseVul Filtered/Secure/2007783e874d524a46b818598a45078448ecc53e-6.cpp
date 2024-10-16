string t_go_generator::type_to_go_type_with_opt(t_type* type,
                                                bool optional_field) {
  string maybe_pointer(optional_field ? "*" : "");

  if (type->is_typedef() && ((t_typedef*)type)->is_forward_typedef()) {
    type = ((t_typedef*)type)->get_true_type();
  }

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();

    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "";

    case t_base_type::TYPE_STRING:
      if (((t_base_type*)type)->is_binary()) {
        return maybe_pointer + "[]byte";
      }

      return maybe_pointer + "string";

    case t_base_type::TYPE_BOOL:
      return maybe_pointer + "bool";

    case t_base_type::TYPE_I8:
      return maybe_pointer + "int8";

    case t_base_type::TYPE_I16:
      return maybe_pointer + "int16";

    case t_base_type::TYPE_I32:
      return maybe_pointer + "int32";

    case t_base_type::TYPE_I64:
      return maybe_pointer + "int64";

    case t_base_type::TYPE_DOUBLE:
      return maybe_pointer + "float64";
    }
  } else if (type->is_enum()) {
    return maybe_pointer + publicize(type_name(type));
  } else if (type->is_struct() || type->is_xception()) {
    return "*" + publicize(type_name(type));
  } else if (type->is_map()) {
    t_map* t = (t_map*)type;
    string keyType = type_to_go_key_type(t->get_key_type());
    string valueType = type_to_go_type(t->get_val_type());
    return maybe_pointer + string("map[") + keyType + "]" + valueType;
  } else if (type->is_set()) {
    t_set* t = (t_set*)type;
    string elemType = type_to_go_key_type(t->get_elem_type());
    return maybe_pointer + string("map[") + elemType + string("]struct{}");
  } else if (type->is_list()) {
    t_list* t = (t_list*)type;
    string elemType = type_to_go_type(t->get_elem_type());
    return maybe_pointer + string("[]") + elemType;
  } else if (type->is_typedef()) {
    return maybe_pointer + publicize(type_name(type));
  }

  throw "INVALID TYPE IN type_to_go_type: " + type->get_name();
}