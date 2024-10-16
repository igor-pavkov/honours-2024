my_bool opt_flush_ok_packet(MYSQL *mysql, my_bool *is_ok_packet)
{
  ulong packet_length= cli_safe_read(mysql);

  if (packet_length == packet_error)
    return TRUE;

  /* cli_safe_read always reads a non-empty packet. */
  DBUG_ASSERT(packet_length);

  *is_ok_packet= mysql->net.read_pos[0] == 0;
  if (*is_ok_packet)
  {
    uchar *pos= mysql->net.read_pos + 1;

    net_field_length_ll(&pos); /* affected rows */
    net_field_length_ll(&pos); /* insert id */

    mysql->server_status=uint2korr(pos);
    pos+=2;

    if (protocol_41(mysql))
    {
      mysql->warning_count=uint2korr(pos);
      pos+=2;
    }
  }
  return FALSE;
}