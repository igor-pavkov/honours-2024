  void dump_xml(Formatter *f) const {
    encode_xml("Role", role, f);
    encode_xml("Rule", rules, f);
  }