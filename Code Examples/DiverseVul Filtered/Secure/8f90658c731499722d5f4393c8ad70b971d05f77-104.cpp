    void dump_xml(Formatter *f) const {
      encode_xml("DeleteMarkerReplication", delete_marker_replication, f);
      encode_xml("Source", source, f);
      encode_xml("Destination", destination, f);
      encode_xml("Filter", filter, f);
      encode_xml("ID", id, f);
      encode_xml("Priority", priority, f);
      encode_xml("Status", status, f);
    }