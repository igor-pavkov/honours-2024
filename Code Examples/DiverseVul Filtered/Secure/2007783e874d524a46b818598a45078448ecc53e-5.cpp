void t_go_generator::generate_deserialize_list_element(ofstream& out,
                                                       t_list* tlist,
                                                       bool declare,
                                                       string prefix) {
  (void)declare;
  string elem = tmp("_elem");
  t_field felem(((t_list*)tlist)->get_elem_type(), elem);
  felem.set_req(t_field::T_OPT_IN_REQ_OUT);
  generate_deserialize_field(out, &felem, true, "", false, false, false, true, true);
  indent(out) << prefix << " = append(" << prefix << ", " << elem << ")" << endl;
}