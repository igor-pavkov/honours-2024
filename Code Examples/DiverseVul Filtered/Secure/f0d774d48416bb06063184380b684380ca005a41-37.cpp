my_bool mysql_reconnect(MYSQL *mysql)
{
  MYSQL tmp_mysql;
  struct my_hook_data hook_data;
  struct mysql_async_context *ctxt= NULL;
  DBUG_ENTER("mysql_reconnect");
  DBUG_ASSERT(mysql);
  DBUG_PRINT("enter", ("mysql->reconnect: %d", mysql->reconnect));

  if (!mysql->reconnect ||
      (mysql->server_status & SERVER_STATUS_IN_TRANS) || !mysql->host_info)
  {
    /* Allow reconnect next time */
    mysql->server_status&= ~SERVER_STATUS_IN_TRANS;
    set_mysql_error(mysql, CR_SERVER_GONE_ERROR, unknown_sqlstate);
    DBUG_RETURN(1);
  }
  mysql_init(&tmp_mysql);
  tmp_mysql.options= mysql->options;
  tmp_mysql.options.my_cnf_file= tmp_mysql.options.my_cnf_group= 0;

  /*
    If we are automatically re-connecting inside a non-blocking API call, we
    may need to suspend and yield to the user application during the reconnect.
    If so, the user application will need access to the new VIO already then
    so that it can correctly wait for I/O to become ready.
    To achieve this, we temporarily install a hook that will temporarily put in
    the VIO while we are suspended.
    (The vio will be put in the original MYSQL permanently once we successfully
    reconnect, or be discarded if we fail to reconnect.)
  */
  if (mysql->options.extension &&
      (ctxt= mysql->options.extension->async_context) &&
      mysql->options.extension->async_context->active)
  {
    hook_data.orig_mysql= mysql;
    hook_data.new_mysql= &tmp_mysql;
    hook_data.orig_vio= mysql->net.vio;
    my_context_install_suspend_resume_hook(ctxt, my_suspend_hook, &hook_data);
  }
  if (!mysql_real_connect(&tmp_mysql,mysql->host,mysql->user,mysql->passwd,
			  mysql->db, mysql->port, mysql->unix_socket,
			  mysql->client_flag))
  {
    if (ctxt)
      my_context_install_suspend_resume_hook(ctxt, NULL, NULL);
    mysql->net.last_errno= tmp_mysql.net.last_errno;
    strmov(mysql->net.last_error, tmp_mysql.net.last_error);
    strmov(mysql->net.sqlstate, tmp_mysql.net.sqlstate);
    DBUG_RETURN(1);
  }
  if (mysql_set_character_set(&tmp_mysql, mysql->charset->csname))
  {
    DBUG_PRINT("error", ("mysql_set_character_set() failed"));
    bzero((char*) &tmp_mysql.options,sizeof(tmp_mysql.options));
    mysql_close(&tmp_mysql);
    if (ctxt)
      my_context_install_suspend_resume_hook(ctxt, NULL, NULL);
    mysql->net.last_errno= tmp_mysql.net.last_errno;
    strmov(mysql->net.last_error, tmp_mysql.net.last_error);
    strmov(mysql->net.sqlstate, tmp_mysql.net.sqlstate);
    DBUG_RETURN(1);
  }
  if (ctxt)
    my_context_install_suspend_resume_hook(ctxt, NULL, NULL);

  DBUG_PRINT("info", ("reconnect succeded"));
  tmp_mysql.reconnect= 1;
  tmp_mysql.free_me= mysql->free_me;

  /* Move prepared statements (if any) over to the new mysql object */
  tmp_mysql.stmts= mysql->stmts;
  mysql->stmts= 0;

  /* Don't free options as these are now used in tmp_mysql */
  bzero((char*) &mysql->options,sizeof(mysql->options));
  mysql->free_me=0;
  mysql_close(mysql);
  *mysql=tmp_mysql;
  net_clear(&mysql->net, 1);
  mysql->affected_rows= ~(my_ulonglong) 0;
  DBUG_RETURN(0);
}