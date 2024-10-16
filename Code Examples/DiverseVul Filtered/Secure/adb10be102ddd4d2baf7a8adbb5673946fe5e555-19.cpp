uint16_t PacketReader::get16BitInt(const vector<unsigned char>&content, uint16_t& pos)
{
  uint16_t ret=0;
  ret+=content.at(pos++);
  ret<<=8;
  ret+=content.at(pos++);
  
  return ret;
}