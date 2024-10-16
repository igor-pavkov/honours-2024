void t_go_generator::generate_deserialize_set_element(ofstream& out,
                                                      t_set* tset,
                                                      bool declare,
                                                      string prefix) {
  (void)declare;
  string elem = tmp("_elem");
  t_field felem(tset->get_elem_type(), elem);
  felem.set_req(t_field::T_OPT_IN_REQ_OUT);
  generate_deserialize_field(out, &felem, true, "", false, false, false, true, true);
  indent(out) << prefix << "[" << elem << "] = struct{}{}" << endl;
}