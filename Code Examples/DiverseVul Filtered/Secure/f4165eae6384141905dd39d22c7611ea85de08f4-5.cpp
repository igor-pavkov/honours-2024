my_bool acl_reload(THD *thd)
{
  TABLE_LIST tables[3];
  DYNAMIC_ARRAY old_acl_users, old_acl_dbs, old_acl_proxy_users;
  MEM_ROOT old_mem;
  bool old_initialized;
  my_bool return_val= TRUE;
  DBUG_ENTER("acl_reload");

  /*
    To avoid deadlocks we should obtain table locks before
    obtaining acl_cache->lock mutex.
  */
  tables[0].init_one_table(C_STRING_WITH_LEN("mysql"),
                           C_STRING_WITH_LEN("user"), "user", TL_READ);
  tables[1].init_one_table(C_STRING_WITH_LEN("mysql"),
                           C_STRING_WITH_LEN("db"), "db", TL_READ);
  tables[2].init_one_table(C_STRING_WITH_LEN("mysql"),
                           C_STRING_WITH_LEN("proxies_priv"), 
                           "proxies_priv", TL_READ);
  tables[0].next_local= tables[0].next_global= tables + 1;
  tables[1].next_local= tables[1].next_global= tables + 2;
  tables[0].open_type= tables[1].open_type= tables[2].open_type= OT_BASE_ONLY;
  tables[2].open_strategy= TABLE_LIST::OPEN_IF_EXISTS;

  if (open_and_lock_tables(thd, tables, FALSE, MYSQL_LOCK_IGNORE_TIMEOUT))
  {
    /*
      Execution might have been interrupted; only print the error message
      if a user error condition has been raised.
    */
    if (thd->get_stmt_da()->is_error())
    {
      sql_print_error("Fatal error: Can't open and lock privilege tables: %s",
                      thd->get_stmt_da()->message());
    }
    goto end;
  }

  if ((old_initialized=initialized))
    mysql_mutex_lock(&acl_cache->lock);

  old_acl_users= acl_users;
  old_acl_proxy_users= acl_proxy_users;
  old_acl_dbs= acl_dbs;
  my_init_dynamic_array(&acl_users, sizeof(ACL_USER), 50, 100);
  my_init_dynamic_array(&acl_dbs, sizeof(ACL_DB), 50, 100);
  my_init_dynamic_array(&acl_proxy_users, sizeof(ACL_PROXY_USER), 50, 100);
  old_mem= global_acl_memory;
  delete_dynamic(&acl_wild_hosts);
  my_hash_free(&acl_check_hosts);

  if ((return_val= acl_load(thd, tables)))
  {					// Error. Revert to old list
    DBUG_PRINT("error",("Reverting to old privileges"));
    acl_free();				/* purecov: inspected */
    acl_users= old_acl_users;
    acl_proxy_users= old_acl_proxy_users;
    acl_dbs= old_acl_dbs;
    global_acl_memory= old_mem;
    init_check_host();
  }
  else
  {
    free_root(&old_mem,MYF(0));
    delete_dynamic(&old_acl_users);
    delete_dynamic(&old_acl_proxy_users);
    delete_dynamic(&old_acl_dbs);
  }
  if (old_initialized)
    mysql_mutex_unlock(&acl_cache->lock);
end:
  close_acl_tables(thd);

  DEBUG_SYNC(thd, "after_acl_reload");
  DBUG_RETURN(return_val);
}