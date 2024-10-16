  uint8_t get8BitInt()
  {
    const char* p = d_packet.c_str() + d_offset;
    moveOffset(1);
    return *p;
  }