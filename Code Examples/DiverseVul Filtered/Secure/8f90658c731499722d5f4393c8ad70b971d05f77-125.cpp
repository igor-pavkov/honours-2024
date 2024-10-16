  bool xml_end(const char *el) override {
    if (!el)
      return false;

    location_constraint = get_data();

    return true;
  }