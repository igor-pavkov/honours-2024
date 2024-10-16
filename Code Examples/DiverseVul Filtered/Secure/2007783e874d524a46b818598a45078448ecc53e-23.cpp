void t_go_generator::generate_serialize_set_element(ofstream& out, t_set* tset, string prefix) {
  t_field efield(tset->get_elem_type(), "");
  efield.set_req(t_field::T_OPT_IN_REQ_OUT);
  generate_serialize_field(out, &efield, prefix);
}