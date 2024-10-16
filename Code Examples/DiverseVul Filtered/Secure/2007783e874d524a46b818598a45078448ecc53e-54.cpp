void t_go_generator::generate_deserialize_field(ofstream& out,
                                                t_field* tfield,
                                                bool declare,
                                                string prefix,
                                                bool inclass,
                                                bool coerceData,
                                                bool inkey,
                                                bool in_container_value,
                                                bool use_true_type) {
  (void)inclass;
  (void)coerceData;
  t_type* orig_type = tfield->get_type();
  t_type* type = get_true_type(orig_type);
  string name(prefix + publicize(tfield->get_name()));

  if (type->is_void()) {
    throw "CANNOT GENERATE DESERIALIZE CODE FOR void TYPE: " + name;
  }

  if (type->is_struct() || type->is_xception()) {
    generate_deserialize_struct(out,
                                (t_struct*)type,
                                is_pointer_field(tfield, in_container_value),
                                declare,
                                name);
  } else if (type->is_container()) {
    generate_deserialize_container(out, orig_type, is_pointer_field(tfield), declare, name);
  } else if (type->is_base_type() || type->is_enum()) {

    if (declare) {
      t_type* actual_type = use_true_type ? tfield->get_type()->get_true_type()
                                          : tfield->get_type();

      string type_name = inkey ? type_to_go_key_type(actual_type) : type_to_go_type(actual_type);

      out << "var " << tfield->get_name() << " " << type_name << endl;
    }

    indent(out) << "if v, err := iprot.";

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();

      switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw "compiler error: cannot serialize void field in a struct: " + name;
        break;

      case t_base_type::TYPE_STRING:
        if (((t_base_type*)type)->is_binary() && !inkey) {
          out << "ReadBinary()";
        } else {
          out << "ReadString()";
        }

        break;

      case t_base_type::TYPE_BOOL:
        out << "ReadBool()";
        break;

      case t_base_type::TYPE_I8:
        out << "ReadByte()";
        break;

      case t_base_type::TYPE_I16:
        out << "ReadI16()";
        break;

      case t_base_type::TYPE_I32:
        out << "ReadI32()";
        break;

      case t_base_type::TYPE_I64:
        out << "ReadI64()";
        break;

      case t_base_type::TYPE_DOUBLE:
        out << "ReadDouble()";
        break;

      default:
        throw "compiler error: no Go name for base type " + t_base_type::t_base_name(tbase);
      }
    } else if (type->is_enum()) {
      out << "ReadI32()";
    }

    out << "; err != nil {" << endl;
    out << indent() << "return thrift.PrependError(\"error reading field " << tfield->get_key()
        << ": \", err)" << endl;

    out << "} else {" << endl;
    string wrap;

    if (type->is_enum() || (orig_type->is_typedef() && !use_true_type)) {
      wrap = publicize(type_name(orig_type));
    } else if (((t_base_type*)type)->get_base() == t_base_type::TYPE_I8) {
      wrap = "int8";
    }

    string maybe_address = (is_pointer_field(tfield) ? "&" : "");
    if (wrap == "") {
      indent(out) << name << " = " << maybe_address << "v" << endl;
    } else {
      indent(out) << "temp := " << wrap << "(v)" << endl;
      indent(out) << name << " = " << maybe_address << "temp" << endl;
    }

    out << "}" << endl;
  } else {
    throw "INVALID TYPE IN generate_deserialize_field '" + type->get_name() + "' for field '"
        + tfield->get_name() + "'";
  }
}