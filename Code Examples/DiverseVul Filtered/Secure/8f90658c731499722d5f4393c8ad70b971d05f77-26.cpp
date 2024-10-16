      void dump_xml(Formatter *f) const {
        encode_xml("Prefix", prefix, f);
        encode_xml("Tag", tag, f);
        encode_xml("And", and_elements, f);
      }