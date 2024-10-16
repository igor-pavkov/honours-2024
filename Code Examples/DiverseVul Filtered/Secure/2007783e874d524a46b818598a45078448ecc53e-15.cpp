void t_go_generator::generate_go_function_helpers(t_function* tfunction) {
  if (!tfunction->is_oneway()) {
    t_struct result(program_, tfunction->get_name() + "_result");
    t_field success(tfunction->get_returntype(), "success", 0);
    success.set_req(t_field::T_OPTIONAL);

    if (!tfunction->get_returntype()->is_void()) {
      result.append(&success);
    }

    t_struct* xs = tfunction->get_xceptions();
    const vector<t_field*>& fields = xs->get_members();
    vector<t_field*>::const_iterator f_iter;

    for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
      t_field* f = *f_iter;
      f->set_req(t_field::T_OPTIONAL);
      result.append(f);
    }

    generate_go_struct_definition(f_types_, &result, false, true);
  }
}