static void setpacketevent(tftp_packet_t *packet, unsigned short num)
{
  packet->data[0] = (unsigned char)(num >> 8);
  packet->data[1] = (unsigned char)(num & 0xff);
}