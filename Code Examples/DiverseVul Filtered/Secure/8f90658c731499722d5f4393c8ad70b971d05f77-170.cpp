      void decode_xml(XMLObj *obj) {
        RGWXMLDecoder::decode_xml("Prefix", prefix, obj);
        if (prefix && prefix->empty()) {
          prefix.reset();
        }
        RGWXMLDecoder::decode_xml("Tag", tag, obj);
        if (tag && tag->empty()) {
          tag.reset();
        }
        RGWXMLDecoder::decode_xml("And", and_elements, obj);
        if (and_elements && and_elements->empty()) {
          and_elements.reset();
        }
      };