void PacketReader::copyRecord(unsigned char* dest, uint16_t len)
{
  if(d_pos + len > d_content.size())
    throw std::out_of_range("Attempt to copy outside of packet");

  memcpy(dest, &d_content.at(d_pos), len);
  d_pos+=len;
}