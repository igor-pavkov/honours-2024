static void setpacketblock(tftp_packet_t *packet, unsigned short num)
{
  packet->data[2] = (unsigned char)(num >> 8);
  packet->data[3] = (unsigned char)(num & 0xff);
}