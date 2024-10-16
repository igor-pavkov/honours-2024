uint8_t PacketReader::get8BitInt()
{
  return d_content.at(d_pos++);
}