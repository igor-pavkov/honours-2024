        void decode_xml(XMLObj *obj) {
          std::vector<Tag> _tags;
          RGWXMLDecoder::decode_xml("Prefix", prefix, obj);
          if (prefix && prefix->empty()) {
            prefix.reset();
          }
          RGWXMLDecoder::decode_xml("Tag", _tags, obj);
          for (auto& t : _tags) {
            if (!t.empty()) {
              tags.push_back(std::move(t));
            }
          }
        };