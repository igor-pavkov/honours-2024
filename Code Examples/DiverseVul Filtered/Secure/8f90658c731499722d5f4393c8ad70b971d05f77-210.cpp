  void decode_xml(XMLObj *obj) {
    RGWXMLDecoder::decode_xml("Role", role, obj);
    RGWXMLDecoder::decode_xml("Rule", rules, obj);
  }