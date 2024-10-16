int run_plugin_auth(MYSQL *mysql, char *data, uint data_len,
                    const char *data_plugin, const char *db)
{
  const char    *auth_plugin_name;
  auth_plugin_t *auth_plugin;
  MCPVIO_EXT    mpvio;
  ulong		pkt_length;
  int           res;

  DBUG_ENTER ("run_plugin_auth");
  /* determine the default/initial plugin to use */
  if (mysql->options.extension && mysql->options.extension->default_auth &&
      mysql->server_capabilities & CLIENT_PLUGIN_AUTH)
  {
    auth_plugin_name= mysql->options.extension->default_auth;
    if (!(auth_plugin= (auth_plugin_t*) mysql_client_find_plugin(mysql,
                       auth_plugin_name, MYSQL_CLIENT_AUTHENTICATION_PLUGIN)))
      DBUG_RETURN (1); /* oops, not found */
  }
  else
  {
    auth_plugin= mysql->server_capabilities & CLIENT_PROTOCOL_41 ?
      &native_password_client_plugin : &old_password_client_plugin;
    auth_plugin_name= auth_plugin->name;
  }

  DBUG_PRINT ("info", ("using plugin %s", auth_plugin_name));

  mysql->net.last_errno= 0; /* just in case */

  if (data_plugin && strcmp(data_plugin, auth_plugin_name))
  {
    /* data was prepared for a different plugin, don't show it to this one */
    data= 0;
    data_len= 0;
  }

  mpvio.mysql_change_user= data_plugin == 0;
  mpvio.cached_server_reply.pkt= (uchar*)data;
  mpvio.cached_server_reply.pkt_len= data_len;
  mpvio.read_packet= client_mpvio_read_packet;
  mpvio.write_packet= client_mpvio_write_packet;
  mpvio.info= client_mpvio_info;
  mpvio.mysql= mysql;
  mpvio.packets_read= mpvio.packets_written= 0;
  mpvio.db= db;
  mpvio.plugin= auth_plugin;

  res= auth_plugin->authenticate_user((struct st_plugin_vio *)&mpvio, mysql);
  DBUG_PRINT ("info", ("authenticate_user returned %s", 
                       res == CR_OK ? "CR_OK" : 
                       res == CR_ERROR ? "CR_ERROR" :
                       res == CR_OK_HANDSHAKE_COMPLETE ? 
                         "CR_OK_HANDSHAKE_COMPLETE" : "error"));

  compile_time_assert(CR_OK == -1);
  compile_time_assert(CR_ERROR == 0);
  if (res > CR_OK && mysql->net.read_pos[0] != 254)
  {
    /*
      the plugin returned an error. write it down in mysql,
      unless the error code is CR_ERROR and mysql->net.last_errno
      is already set (the plugin has done it)
    */
    DBUG_PRINT ("info", ("res=%d", res));
    if (res > CR_ERROR)
      set_mysql_error(mysql, res, unknown_sqlstate);
    else
      if (!mysql->net.last_errno)
        set_mysql_error(mysql, CR_UNKNOWN_ERROR, unknown_sqlstate);
    DBUG_RETURN (1);
  }

  /* read the OK packet (or use the cached value in mysql->net.read_pos */
  if (res == CR_OK)
    pkt_length= (*mysql->methods->read_change_user_result)(mysql);
  else /* res == CR_OK_HANDSHAKE_COMPLETE */
    pkt_length= mpvio.last_read_packet_len;

  DBUG_PRINT ("info", ("OK packet length=%lu", pkt_length));
  if (pkt_length == packet_error)
  {
    if (mysql->net.last_errno == CR_SERVER_LOST)
      set_mysql_extended_error(mysql, CR_SERVER_LOST, unknown_sqlstate,
                               ER(CR_SERVER_LOST_EXTENDED),
                               "reading authorization packet",
                               errno);
    DBUG_RETURN (1);
  }

  if (mysql->net.read_pos[0] == 254)
  {
    /* The server asked to use a different authentication plugin */
    if (pkt_length == 1)
    {
      /* old "use short scramble" packet */
      DBUG_PRINT ("info", ("old use short scramble packet from server"));
      auth_plugin_name= old_password_plugin_name;
      mpvio.cached_server_reply.pkt= (uchar*)mysql->scramble;
      mpvio.cached_server_reply.pkt_len= SCRAMBLE_LENGTH + 1;
    }
    else
    {
      /* new "use different plugin" packet */
      uint len;
      auth_plugin_name= (char*)mysql->net.read_pos + 1;
      len= strlen(auth_plugin_name); /* safe as my_net_read always appends \0 */
      mpvio.cached_server_reply.pkt_len= pkt_length - len - 2;
      mpvio.cached_server_reply.pkt= mysql->net.read_pos + len + 2;
      DBUG_PRINT ("info", ("change plugin packet from server for plugin %s",
                           auth_plugin_name));
    }

    if (!(auth_plugin= (auth_plugin_t *) mysql_client_find_plugin(mysql,
                         auth_plugin_name, MYSQL_CLIENT_AUTHENTICATION_PLUGIN)))
      DBUG_RETURN (1);

    mpvio.plugin= auth_plugin;
    res= auth_plugin->authenticate_user((struct st_plugin_vio *)&mpvio, mysql);

    DBUG_PRINT ("info", ("second authenticate_user returned %s", 
                         res == CR_OK ? "CR_OK" : 
                         res == CR_ERROR ? "CR_ERROR" :
                         res == CR_OK_HANDSHAKE_COMPLETE ? 
                         "CR_OK_HANDSHAKE_COMPLETE" : "error"));
    if (res > CR_OK)
    {
      if (res > CR_ERROR)
        set_mysql_error(mysql, res, unknown_sqlstate);
      else
        if (!mysql->net.last_errno)
          set_mysql_error(mysql, CR_UNKNOWN_ERROR, unknown_sqlstate);
      DBUG_RETURN (1);
    }

    if (res != CR_OK_HANDSHAKE_COMPLETE)
    {
      /* Read what server thinks about out new auth message report */
      if (cli_safe_read(mysql) == packet_error)
      {
        if (mysql->net.last_errno == CR_SERVER_LOST)
          set_mysql_extended_error(mysql, CR_SERVER_LOST, unknown_sqlstate,
                                   ER(CR_SERVER_LOST_EXTENDED),
                                   "reading final connect information",
                                   errno);
        DBUG_RETURN (1);
      }
    }
  }
  /*
    net->read_pos[0] should always be 0 here if the server implements
    the protocol correctly
  */
  DBUG_RETURN (mysql->net.read_pos[0] != 0);
}