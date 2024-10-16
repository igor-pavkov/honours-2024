  explicit DNSPacketMangler(std::string& packet)
    : d_packet(packet), d_notyouroffset(12), d_offset(d_notyouroffset)
  {}