void t_go_generator::generate_deserialize_map_element(ofstream& out,
                                                      t_map* tmap,
                                                      bool declare,
                                                      string prefix) {
  (void)declare;
  string key = tmp("_key");
  string val = tmp("_val");
  t_field fkey(tmap->get_key_type(), key);
  t_field fval(tmap->get_val_type(), val);
  fkey.set_req(t_field::T_OPT_IN_REQ_OUT);
  fval.set_req(t_field::T_OPT_IN_REQ_OUT);
  generate_deserialize_field(out, &fkey, true, "", false, false, true);
  generate_deserialize_field(out, &fval, true, "", false, false, false, true);
  indent(out) << prefix << "[" << key << "] = " << val << endl;
}