void t_go_generator::generate_serialize_map_element(ofstream& out,
                                                    t_map* tmap,
                                                    string kiter,
                                                    string viter) {
  t_field kfield(tmap->get_key_type(), "");
  t_field vfield(tmap->get_val_type(), "");
  kfield.set_req(t_field::T_OPT_IN_REQ_OUT);
  vfield.set_req(t_field::T_OPT_IN_REQ_OUT);
  generate_serialize_field(out, &kfield, kiter, true);
  generate_serialize_field(out, &vfield, viter);
}