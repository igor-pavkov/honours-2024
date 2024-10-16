        void decode_xml(XMLObj *obj) {
          RGWXMLDecoder::decode_xml("Key", key, obj);
          RGWXMLDecoder::decode_xml("Value", value, obj);
        };