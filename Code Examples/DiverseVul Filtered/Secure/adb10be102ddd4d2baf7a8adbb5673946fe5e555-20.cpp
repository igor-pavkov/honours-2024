  void decreaseAndSkip32BitInt(uint32_t decrease)
  {
    const char *p = (const char*)d_packet.c_str() + d_offset;
    moveOffset(4);
    
    uint32_t tmp;
    memcpy(&tmp, (void*) p, sizeof(tmp));
    tmp = ntohl(tmp);
    tmp-=decrease;
    tmp = htonl(tmp);
    d_packet.replace(d_offset-4, sizeof(tmp), (const char*)&tmp, sizeof(tmp));
  }