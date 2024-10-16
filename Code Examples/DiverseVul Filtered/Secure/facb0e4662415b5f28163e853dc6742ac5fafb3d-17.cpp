static unsigned short getrpacketblock(const tftp_packet_t *packet)
{
  return (unsigned short)((packet->data[2] << 8) | packet->data[3]);
}