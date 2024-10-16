static void mysql_close_free_options(MYSQL *mysql)
{
  DBUG_ENTER("mysql_close_free_options");

  my_free(mysql->options.user);
  my_free(mysql->options.host);
  my_free(mysql->options.password);
  my_free(mysql->options.unix_socket);
  my_free(mysql->options.db);
  my_free(mysql->options.my_cnf_file);
  my_free(mysql->options.my_cnf_group);
  my_free(mysql->options.charset_dir);
  my_free(mysql->options.charset_name);
  my_free(mysql->options.client_ip);
  if (mysql->options.init_commands)
  {
    DYNAMIC_ARRAY *init_commands= mysql->options.init_commands;
    char **ptr= (char**)init_commands->buffer;
    char **end= ptr + init_commands->elements;
    for (; ptr<end; ptr++)
      my_free(*ptr);
    delete_dynamic(init_commands);
    my_free(init_commands);
  }
#if defined(HAVE_OPENSSL) && !defined(EMBEDDED_LIBRARY)
  mysql_ssl_free(mysql);
#endif /* HAVE_OPENSSL && !EMBEDDED_LIBRARY */
#ifdef HAVE_SMEM
  if (mysql->options.shared_memory_base_name != def_shared_memory_base_name)
    my_free(mysql->options.shared_memory_base_name);
#endif /* HAVE_SMEM */
  if (mysql->options.extension)
  {
    struct mysql_async_context *ctxt= mysql->options.extension->async_context;
    my_free(mysql->options.extension->plugin_dir);
    my_free(mysql->options.extension->default_auth);
    if (ctxt)
    {
      my_context_destroy(&ctxt->async_context);
      my_free(ctxt);
    }
    my_free(mysql->options.extension);
  }
  bzero((char*) &mysql->options,sizeof(mysql->options));
  DBUG_VOID_RETURN;
}