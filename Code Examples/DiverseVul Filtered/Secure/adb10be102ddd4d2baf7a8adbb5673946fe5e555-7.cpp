  uint16_t get16BitInt()
  {
    const char* p = d_packet.c_str() + d_offset;
    moveOffset(2);
    uint16_t ret;
    memcpy(&ret, (void*)p, 2);
    return ntohs(ret);
  }