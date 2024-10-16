string t_go_generator::type_to_enum(t_type* type) {
  type = get_true_type(type);

  if (type->is_base_type()) {
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();

    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "NO T_VOID CONSTRUCT";

    case t_base_type::TYPE_STRING:
      /* this is wrong, binary is still a string type internally
      if (((t_base_type*)type)->is_binary()) {
          return "thrift.BINARY";
      }
      */
      return "thrift.STRING";

    case t_base_type::TYPE_BOOL:
      return "thrift.BOOL";

    case t_base_type::TYPE_I8:
      return "thrift.BYTE";

    case t_base_type::TYPE_I16:
      return "thrift.I16";

    case t_base_type::TYPE_I32:
      return "thrift.I32";

    case t_base_type::TYPE_I64:
      return "thrift.I64";

    case t_base_type::TYPE_DOUBLE:
      return "thrift.DOUBLE";
    }
  } else if (type->is_enum()) {
    return "thrift.I32";
  } else if (type->is_struct() || type->is_xception()) {
    return "thrift.STRUCT";
  } else if (type->is_map()) {
    return "thrift.MAP";
  } else if (type->is_set()) {
    return "thrift.SET";
  } else if (type->is_list()) {
    return "thrift.LIST";
  }

  throw "INVALID TYPE IN type_to_enum: " + type->get_name();
}