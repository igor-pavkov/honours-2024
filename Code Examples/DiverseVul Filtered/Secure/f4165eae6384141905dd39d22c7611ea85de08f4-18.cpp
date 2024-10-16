bool change_password(THD *thd, const char *host, const char *user,
		     char *new_password)
{
  TABLE_LIST tables;
  TABLE *table;
  Acl_table_intact table_intact;
  /* Buffer should be extended when password length is extended. */
  char buff[2048];
  ulong query_length;
  bool save_binlog_row_based;
  uchar user_key[MAX_KEY_LENGTH];
  char *plugin_temp= NULL;
  bool plugin_empty;
  char *new_escaped_password= NULL;
  uint new_password_len= (uint) strlen(new_password);
  bool result= 1;
  enum mysql_user_table_field password_field= MYSQL_USER_FIELD_PASSWORD;
  DBUG_ENTER("change_password");
  DBUG_PRINT("enter",("host: '%s'  user: '%s'  new_password: '%s'",
		      host,user,new_password));
  DBUG_ASSERT(host != 0);			// Ensured by parent

  if (check_change_password(thd, host, user, new_password, new_password_len))
    DBUG_RETURN(1);

  tables.init_one_table("mysql", 5, "user", 4, "user", TL_WRITE);

#ifdef HAVE_REPLICATION
  /*
    GRANT and REVOKE are applied the slave in/exclusion rules as they are
    some kind of updates to the mysql.% tables.
  */
  if (thd->slave_thread && rpl_filter->is_on())
  {
    /*
      The tables must be marked "updating" so that tables_ok() takes them into
      account in tests.  It's ok to leave 'updating' set after tables_ok.
    */
    tables.updating= 1;
    /* Thanks to memset, tables.next==0 */
    if (!(thd->sp_runtime_ctx || rpl_filter->tables_ok(0, &tables)))
      DBUG_RETURN(0);
  }
#endif
  if (!(table= open_ltable(thd, &tables, TL_WRITE, MYSQL_LOCK_IGNORE_TIMEOUT)))
    DBUG_RETURN(1);

  if (table_intact.check(table, &mysql_user_table_def))
    DBUG_RETURN(1);

  if (!table->key_info)
  {
    my_error(ER_TABLE_CORRUPT, MYF(0), table->s->db.str,
             table->s->table_name.str);
    DBUG_RETURN(1);
  }

  /*
    This statement will be replicated as a statement, even when using
    row-based replication.  The flag will be reset at the end of the
    statement.
  */
  if ((save_binlog_row_based= thd->is_current_stmt_binlog_format_row()))
    thd->clear_current_stmt_binlog_format_row();

  mysql_mutex_lock(&acl_cache->lock);
  ACL_USER *acl_user;
  if (!(acl_user= find_acl_user(host, user, TRUE)))
  {
    mysql_mutex_unlock(&acl_cache->lock);
    my_message(ER_PASSWORD_NO_MATCH, ER(ER_PASSWORD_NO_MATCH), MYF(0));
    goto end;
  }
  mysql_mutex_assert_owner(&acl_cache->lock);
  table->use_all_columns();
  DBUG_ASSERT(host != '\0');
  table->field[MYSQL_USER_FIELD_HOST]->store(host, strlen(host),
                                             system_charset_info);
  table->field[MYSQL_USER_FIELD_USER]->store(user, strlen(user),
                                             system_charset_info);

  key_copy((uchar *) user_key, table->record[0], table->key_info,
           table->key_info->key_length);
  if (!table->file->ha_index_read_idx_map(table->record[0], 0, user_key,
                                          HA_WHOLE_KEY,
                                          HA_READ_KEY_EXACT))
    plugin_temp= (table->s->fields > MYSQL_USER_FIELD_PLUGIN) ?
                 get_field(&global_acl_memory, table->field[MYSQL_USER_FIELD_PLUGIN]) : NULL;
  else
  {
    my_message(ER_PASSWORD_NO_MATCH, ER(ER_PASSWORD_NO_MATCH), MYF(0));
    mysql_mutex_unlock(&acl_cache->lock);
    goto end;
  }

  plugin_empty= plugin_temp ? false: true;

  if (acl_user->plugin.length == 0)
  {
    acl_user->plugin.length= default_auth_plugin_name.length;
    acl_user->plugin.str= default_auth_plugin_name.str;
  }

  if (new_password_len == 0)
  {
    String *password_str= new (thd->mem_root) String(new_password,
                                                     thd->variables.
                                                     character_set_client);
    if (check_password_policy(password_str))
    {
      result= 1;
      mysql_mutex_unlock(&acl_cache->lock);
      goto end;
    }
  }
  
#if defined(HAVE_OPENSSL)
  /*
    update loaded acl entry:
    TODO Should password depend on @@old_variables here?
    - Probably not if the user exists and have a plugin set already.
  */
  if (my_strcasecmp(system_charset_info, acl_user->plugin.str,
                    sha256_password_plugin_name.str) == 0)
  {
    /*
     Accept empty passwords
    */
    if (new_password_len == 0)
    {
      /*
        Since we're changing the password for the user we need to reset the
        expiration flag.
      */
      if (!update_sctx_cache(thd->security_ctx, acl_user, false) &&
          thd->security_ctx->password_expired)
      {
        /* the current user is not the same as the user we operate on */
        my_error(ER_MUST_CHANGE_PASSWORD, MYF(0));
        result= 1;
        mysql_mutex_unlock(&acl_cache->lock);
        goto end;
      }
      acl_user->password_expired= false;
      acl_user->auth_string= empty_lex_str;
    }
    /*
     Check if password begins with correct magic number
    */
    else if (new_password[0] == '$' &&
             new_password[1] == '5' &&
             new_password[2] == '$')
    {
      password_field= MYSQL_USER_FIELD_AUTHENTICATION_STRING;
      if (new_password_len < CRYPT_MAX_PASSWORD_SIZE + 1)
      {
        /*
          Since we're changing the password for the user we need to reset the
          expiration flag.
        */
        if (!update_sctx_cache(thd->security_ctx, acl_user, false) &&
            thd->security_ctx->password_expired)
        {
          /* the current user is not the same as the user we operate on */
          my_error(ER_MUST_CHANGE_PASSWORD, MYF(0));
          result= 1;
          mysql_mutex_unlock(&acl_cache->lock);
          goto end;
        }

        acl_user->password_expired= false;
        /* copy string including \0 */
        acl_user->auth_string.str= (char *) memdup_root(&global_acl_memory,
                                                       new_password,
                                                       new_password_len + 1);
        acl_user->auth_string.length= new_password_len;
      }
    } else
    {
      /*
        Password format is unexpected. The user probably is using the wrong
        password algorithm with the PASSWORD() function.
      */
      my_error(ER_PASSWORD_FORMAT, MYF(0));
      result= 1;
      mysql_mutex_unlock(&acl_cache->lock);
      goto end;
    }
  }
  else
#endif
  if (my_strcasecmp(system_charset_info, acl_user->plugin.str,
                    native_password_plugin_name.str) == 0 ||
      my_strcasecmp(system_charset_info, acl_user->plugin.str,
                    old_password_plugin_name.str) == 0)
  {
    password_field= MYSQL_USER_FIELD_PASSWORD;
    
    /*
      Legacy code produced an error if the password hash didn't match the
      expectations.
    */
    if (new_password_len != 0)
    {
      if (plugin_empty)
      {
        if (new_password_len == SCRAMBLED_PASSWORD_CHAR_LENGTH)
          acl_user->plugin= native_password_plugin_name;
        else if (new_password_len == SCRAMBLED_PASSWORD_CHAR_LENGTH_323)
          acl_user->plugin= old_password_plugin_name;
        else
        {
          my_error(ER_PASSWD_LENGTH, MYF(0), SCRAMBLED_PASSWORD_CHAR_LENGTH);
          result= 1;
          mysql_mutex_unlock(&acl_cache->lock);
          goto end;
        }
      }
      else
      {
        if (my_strcasecmp(system_charset_info, acl_user->plugin.str,
                          native_password_plugin_name.str) == 0 &&
            new_password_len != SCRAMBLED_PASSWORD_CHAR_LENGTH)
        {
          my_error(ER_PASSWD_LENGTH, MYF(0), SCRAMBLED_PASSWORD_CHAR_LENGTH);
          result= 1;
          mysql_mutex_unlock(&acl_cache->lock);
          goto end;
        }
        else if (my_strcasecmp(system_charset_info, acl_user->plugin.str,
                               old_password_plugin_name.str) == 0 &&
                 new_password_len != SCRAMBLED_PASSWORD_CHAR_LENGTH_323)
        {
          my_error(ER_PASSWD_LENGTH, MYF(0), SCRAMBLED_PASSWORD_CHAR_LENGTH_323);
          result= 1;
          mysql_mutex_unlock(&acl_cache->lock);
          goto end;
        }
      }
    }
    else if (plugin_empty)
      acl_user->plugin= native_password_plugin_name;

    /*
      Update loaded acl entry in memory.
      set_user_salt() stores a binary (compact) representation of the password
      in memory (acl_user->salt and salt_len).
      set_user_plugin() sets the appropriate plugin based on password length and
      if the length doesn't match a warning is issued.
    */
    if (set_user_salt(acl_user, new_password, new_password_len))
    {
      my_error(ER_PASSWORD_FORMAT, MYF(0));
      result= 1;
      mysql_mutex_unlock(&acl_cache->lock);
      goto end;
    }
    if (!update_sctx_cache(thd->security_ctx, acl_user, false) &&
        thd->security_ctx->password_expired)
    {
      my_error(ER_MUST_CHANGE_PASSWORD, MYF(0));
      result= 1;
      mysql_mutex_unlock(&acl_cache->lock);
      goto end;
    }
  }
  else
  {
     push_warning(thd, Sql_condition::WARN_LEVEL_NOTE,
                  ER_SET_PASSWORD_AUTH_PLUGIN, ER(ER_SET_PASSWORD_AUTH_PLUGIN));
     /*
       An undefined password factory could very well mean that the password
       field is empty.
     */
     new_password_len= 0;
  }

  if (update_user_table(thd, table,
                        acl_user->host.get_host() ? acl_user->host.get_host() : "",
                        acl_user->user ? acl_user->user : "",
                        new_password, new_password_len, password_field, false, true))
  {
    mysql_mutex_unlock(&acl_cache->lock); /* purecov: deadcode */
    goto end;
  }

  acl_cache->clear(1);				// Clear locked hostname cache
  mysql_mutex_unlock(&acl_cache->lock);
  result= 0;
  /*
     Before writing the query to binlog, correctly escape the password
     string so that the slave can parse it correctly.
  */

  new_escaped_password= (char *)alloc_root(thd->mem_root, (new_password_len*2+1));
  if (!new_escaped_password)
  {
    my_error(ER_OUTOFMEMORY, MYF(ME_FATALERROR), 0);
    result= 1;
    goto end;
  }

  DBUG_EXECUTE_IF("force_hash_string_with_quote",
                   strcpy(new_password, HASH_STRING_WITH_QUOTE);
                 );

  escape_string_mysql(thd, new_escaped_password, new_password, new_password_len);

  query_length= sprintf(buff, "SET PASSWORD FOR '%-.120s'@'%-.120s'='%-.120s'",
                        acl_user->user ? acl_user->user : "",
                        acl_user->host.get_host() ? acl_user->host.get_host() : "",
                        new_escaped_password);
  result= write_bin_log(thd, true, buff, query_length,
                        table->file->has_transactions());
end:
  result|= acl_trans_commit_and_close_tables(thd);

  /* Restore the state of binlog format */
  DBUG_ASSERT(!thd->is_current_stmt_binlog_format_row());
  if (save_binlog_row_based)
    thd->set_current_stmt_binlog_format_row();

  DBUG_RETURN(result);
}