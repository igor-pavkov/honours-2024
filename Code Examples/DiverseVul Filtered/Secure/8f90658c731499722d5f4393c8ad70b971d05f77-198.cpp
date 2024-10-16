        void dump_xml(Formatter *f) const {
          encode_xml("Key", key, f);
          encode_xml("Value", value, f);
        }