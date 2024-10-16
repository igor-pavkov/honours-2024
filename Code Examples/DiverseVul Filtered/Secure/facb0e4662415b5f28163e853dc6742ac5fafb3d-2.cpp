static unsigned short getrpacketevent(const tftp_packet_t *packet)
{
  return (unsigned short)((packet->data[0] << 8) | packet->data[1]);
}