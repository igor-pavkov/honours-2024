string t_go_generator::function_signature_if(t_function* tfunction, string prefix, bool addError) {
  // TODO(mcslee): Nitpicky, no ',' if argument_list is empty
  string signature = publicize(prefix + tfunction->get_name()) + "(";
  signature += argument_list(tfunction->get_arglist()) + ") (";
  t_type* ret = tfunction->get_returntype();
  t_struct* exceptions = tfunction->get_xceptions();
  string errs = argument_list(exceptions);

  if (!ret->is_void()) {
    signature += "r " + type_to_go_type(ret);

    if (addError || errs.size() == 0) {
      signature += ", ";
    }
  }

  if (addError) {
    signature += "err error";
  }

  signature += ")";
  return signature;
}