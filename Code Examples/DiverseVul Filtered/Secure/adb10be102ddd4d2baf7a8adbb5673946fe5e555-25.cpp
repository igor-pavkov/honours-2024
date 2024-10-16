  void moveOffset(uint16_t by)
  {
    d_notyouroffset += by;
    if(d_notyouroffset > d_packet.length())
      throw std::out_of_range("dns packet out of range: "+lexical_cast<string>(d_notyouroffset) +" > " 
      + lexical_cast<string>(d_packet.length()) );
  }