void t_go_generator::generate_serialize_field(ofstream& out,
                                              t_field* tfield,
                                              string prefix,
                                              bool inkey) {
  t_type* type = get_true_type(tfield->get_type());
  string name(prefix + publicize(tfield->get_name()));

  // Do nothing for void types
  if (type->is_void()) {
    throw "compiler error: cannot generate serialize for void type: " + name;
  }

  if (type->is_struct() || type->is_xception()) {
    generate_serialize_struct(out, (t_struct*)type, name);
  } else if (type->is_container()) {
    generate_serialize_container(out, type, is_pointer_field(tfield), name);
  } else if (type->is_base_type() || type->is_enum()) {
    indent(out) << "if err := oprot.";

    if (is_pointer_field(tfield)) {
      name = "*" + name;
    }

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();

      switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw "compiler error: cannot serialize void field in a struct: " + name;
        break;

      case t_base_type::TYPE_STRING:
        if (((t_base_type*)type)->is_binary() && !inkey) {
          out << "WriteBinary(" << name << ")";
        } else {
          out << "WriteString(string(" << name << "))";
        }

        break;

      case t_base_type::TYPE_BOOL:
        out << "WriteBool(bool(" << name << "))";
        break;

      case t_base_type::TYPE_I8:
        out << "WriteByte(int8(" << name << "))";
        break;

      case t_base_type::TYPE_I16:
        out << "WriteI16(int16(" << name << "))";
        break;

      case t_base_type::TYPE_I32:
        out << "WriteI32(int32(" << name << "))";
        break;

      case t_base_type::TYPE_I64:
        out << "WriteI64(int64(" << name << "))";
        break;

      case t_base_type::TYPE_DOUBLE:
        out << "WriteDouble(float64(" << name << "))";
        break;

      default:
        throw "compiler error: no Go name for base type " + t_base_type::t_base_name(tbase);
      }
    } else if (type->is_enum()) {
      out << "WriteI32(int32(" << name << "))";
    }

    out << "; err != nil {" << endl;
    out << indent() << "return thrift.PrependError(fmt.Sprintf(\"%T."
        << escape_string(tfield->get_name()) << " (" << tfield->get_key()
        << ") field write error: \", p), err) }" << endl;
  } else {
    throw "compiler error: Invalid type in generate_serialize_field '" + type->get_name()
        + "' for field '" + name + "'";
  }
}