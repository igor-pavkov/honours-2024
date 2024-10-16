    void decode_xml(XMLObj *obj) {
      RGWXMLDecoder::decode_xml("DeleteMarkerReplication", delete_marker_replication, obj);
      RGWXMLDecoder::decode_xml("Source", source, obj);
      RGWXMLDecoder::decode_xml("Destination", destination, obj);
      RGWXMLDecoder::decode_xml("ID", id, obj);

      std::optional<string> prefix;
      RGWXMLDecoder::decode_xml("Prefix", prefix, obj);
      if (prefix) {
        filter.emplace();
        filter->prefix = prefix;
      }

      if (!filter) {
        RGWXMLDecoder::decode_xml("Filter", filter, obj);
      } else {
        /* don't want to have filter reset because it might have been initialized
         * when decoding prefix
         */
        RGWXMLDecoder::decode_xml("Filter", *filter, obj);
      }

      RGWXMLDecoder::decode_xml("Priority", priority, obj);
      RGWXMLDecoder::decode_xml("Status", status, obj);
    }