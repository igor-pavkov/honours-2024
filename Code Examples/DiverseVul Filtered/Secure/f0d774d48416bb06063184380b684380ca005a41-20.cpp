static int native_password_auth_client(MYSQL_PLUGIN_VIO *vio, MYSQL *mysql)
{
  int pkt_len;
  uchar *pkt;

  DBUG_ENTER("native_password_auth_client");


  if (((MCPVIO_EXT *)vio)->mysql_change_user)
  {
    /*
      in mysql_change_user() the client sends the first packet.
      we use the old scramble.
    */
    pkt= (uchar*)mysql->scramble;
    pkt_len= SCRAMBLE_LENGTH + 1;
  }
  else
  {
    /* read the scramble */
    if ((pkt_len= vio->read_packet(vio, &pkt)) < 0)
      DBUG_RETURN(CR_ERROR);

    if (pkt_len != SCRAMBLE_LENGTH + 1)
      DBUG_RETURN(CR_SERVER_HANDSHAKE_ERR);

    /* save it in MYSQL */
    memcpy(mysql->scramble, pkt, SCRAMBLE_LENGTH);
    mysql->scramble[SCRAMBLE_LENGTH] = 0;
  }

  if (mysql->passwd[0])
  {
    char scrambled[SCRAMBLE_LENGTH + 1];
    DBUG_PRINT("info", ("sending scramble"));
    scramble(scrambled, (char*)pkt, mysql->passwd);
    if (vio->write_packet(vio, (uchar*)scrambled, SCRAMBLE_LENGTH))
      DBUG_RETURN(CR_ERROR);
  }
  else
  {
    DBUG_PRINT("info", ("no password"));
    if (vio->write_packet(vio, 0, 0)) /* no password */
      DBUG_RETURN(CR_ERROR);
  }

  DBUG_RETURN(CR_OK);
}