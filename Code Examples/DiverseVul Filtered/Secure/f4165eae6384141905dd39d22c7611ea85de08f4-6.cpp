static my_bool acl_load(THD *thd, TABLE_LIST *tables)
{
  TABLE *table;
  READ_RECORD read_record_info;
  my_bool return_val= TRUE;
  bool check_no_resolve= specialflag & SPECIAL_NO_RESOLVE;
  char tmp_name[NAME_LEN+1];
  int password_length;
  char *password;
  uint password_len;
  sql_mode_t old_sql_mode= thd->variables.sql_mode;
  bool password_expired= false;
  DBUG_ENTER("acl_load");

  thd->variables.sql_mode&= ~MODE_PAD_CHAR_TO_FULL_LENGTH;

  grant_version++; /* Privileges updated */

  
  acl_cache->clear(1);				// Clear locked hostname cache

  init_sql_alloc(&global_acl_memory, ACL_ALLOC_BLOCK_SIZE, 0);
  /*
    Prepare reading from the mysql.user table
  */
  if (init_read_record(&read_record_info, thd, table=tables[0].table,
                       NULL, 1, 1, FALSE))
    goto end;
  table->use_all_columns();
  
  allow_all_hosts=0;
  while (!(read_record_info.read_record(&read_record_info)))
  {
    password_expired= false;
    /* Reading record from mysql.user */
    ACL_USER user;
    memset(&user, 0, sizeof(user));
    user.host.update_hostname(get_field(&global_acl_memory,
                                        table->field[MYSQL_USER_FIELD_HOST]));
    user.user= get_field(&global_acl_memory,
                         table->field[MYSQL_USER_FIELD_USER]);

    /*
      All accounts can authenticate per default. This will change when
      we add a new field to the user table.

      Currently this flag is only set to false when authentication is attempted
      using an unknown user name.
    */
    user.can_authenticate= true;

    if (check_no_resolve && hostname_requires_resolving(user.host.get_host()))
    {
      sql_print_warning("'user' entry '%s@%s' "
                        "ignored in --skip-name-resolve mode.",
			user.user ? user.user : "",
			user.host.get_host() ? user.host.get_host() : "");
      continue;
    }

    /* Read legacy password */
    password= get_field(&global_acl_memory,
                        table->field[MYSQL_USER_FIELD_PASSWORD]);
    password_len= password ? strlen(password) : 0;
    user.auth_string.str= password ? password : const_cast<char*>("");
    user.auth_string.length= password_len;

    {
      uint next_field;
      user.access= get_access(table,3,&next_field) & GLOBAL_ACLS;
      /*
        if it is pre 5.0.1 privilege table then map CREATE privilege on
        CREATE VIEW & SHOW VIEW privileges
      */
      if (table->s->fields <= 31 && (user.access & CREATE_ACL))
        user.access|= (CREATE_VIEW_ACL | SHOW_VIEW_ACL);

      /*
        if it is pre 5.0.2 privilege table then map CREATE/ALTER privilege on
        CREATE PROCEDURE & ALTER PROCEDURE privileges
      */
      if (table->s->fields <= 33 && (user.access & CREATE_ACL))
        user.access|= CREATE_PROC_ACL;
      if (table->s->fields <= 33 && (user.access & ALTER_ACL))
        user.access|= ALTER_PROC_ACL;

      /*
        pre 5.0.3 did not have CREATE_USER_ACL
      */
      if (table->s->fields <= 36 && (user.access & GRANT_ACL))
        user.access|= CREATE_USER_ACL;


      /*
        if it is pre 5.1.6 privilege table then map CREATE privilege on
        CREATE|ALTER|DROP|EXECUTE EVENT
      */
      if (table->s->fields <= 37 && (user.access & SUPER_ACL))
        user.access|= EVENT_ACL;

      /*
        if it is pre 5.1.6 privilege then map TRIGGER privilege on CREATE.
      */
      if (table->s->fields <= 38 && (user.access & SUPER_ACL))
        user.access|= TRIGGER_ACL;

      user.sort= get_sort(2,user.host.get_host(),user.user);

      /* Starting from 4.0.2 we have more fields */
      if (table->s->fields >= 31)
      {
        char *ssl_type=
          get_field(thd->mem_root, table->field[MYSQL_USER_FIELD_SSL_TYPE]);
        if (!ssl_type)
          user.ssl_type=SSL_TYPE_NONE;
        else if (!strcmp(ssl_type, "ANY"))
          user.ssl_type=SSL_TYPE_ANY;
        else if (!strcmp(ssl_type, "X509"))
          user.ssl_type=SSL_TYPE_X509;
        else  /* !strcmp(ssl_type, "SPECIFIED") */
          user.ssl_type=SSL_TYPE_SPECIFIED;

        user.ssl_cipher= 
          get_field(&global_acl_memory, table->field[MYSQL_USER_FIELD_SSL_CIPHER]);
        user.x509_issuer=
          get_field(&global_acl_memory, table->field[MYSQL_USER_FIELD_X509_ISSUER]);
        user.x509_subject=
          get_field(&global_acl_memory, table->field[MYSQL_USER_FIELD_X509_SUBJECT]);

        char *ptr= get_field(thd->mem_root,
                             table->field[MYSQL_USER_FIELD_MAX_QUESTIONS]);
        user.user_resource.questions=ptr ? atoi(ptr) : 0;
        ptr= get_field(thd->mem_root,
                       table->field[MYSQL_USER_FIELD_MAX_UPDATES]);
        user.user_resource.updates=ptr ? atoi(ptr) : 0;
        ptr= get_field(thd->mem_root,
                       table->field[MYSQL_USER_FIELD_MAX_CONNECTIONS]);
        user.user_resource.conn_per_hour= ptr ? atoi(ptr) : 0;
        if (user.user_resource.questions || user.user_resource.updates ||
            user.user_resource.conn_per_hour)
          mqh_used=1;

        if (table->s->fields > MYSQL_USER_FIELD_MAX_USER_CONNECTIONS)
        {
          /* Starting from 5.0.3 we have max_user_connections field */
          ptr= get_field(thd->mem_root,
                         table->field[MYSQL_USER_FIELD_MAX_USER_CONNECTIONS]);
          user.user_resource.user_conn= ptr ? atoi(ptr) : 0;
        }

        if (table->s->fields >= 41)
        {
          /* We may have plugin & auth_String fields */
          char *tmpstr= get_field(&global_acl_memory,
                                  table->field[MYSQL_USER_FIELD_PLUGIN]);
          if (tmpstr)
          {
            /*
              By comparing the plugin with the built in plugins it is possible
              to optimize the string allocation and comparision.
            */
            if (my_strcasecmp(system_charset_info, tmpstr,
                              native_password_plugin_name.str) == 0)
              user.plugin= native_password_plugin_name;
            else
              if (my_strcasecmp(system_charset_info, tmpstr,
                                old_password_plugin_name.str) == 0)
                user.plugin= old_password_plugin_name;
#if defined(HAVE_OPENSSL)
            else
              if (my_strcasecmp(system_charset_info, tmpstr,
                                sha256_password_plugin_name.str) == 0)
                user.plugin= sha256_password_plugin_name;
#endif
            else
              {
                user.plugin.str= tmpstr;
                user.plugin.length= strlen(tmpstr);
              }
            if (user.auth_string.length &&
                user.plugin.str != native_password_plugin_name.str &&
                user.plugin.str != old_password_plugin_name.str)
            {
              sql_print_warning("'user' entry '%s@%s' has both a password "
                                "and an authentication plugin specified. The "
                                "password will be ignored.",
                                user.user ? user.user : "",
                                user.host.get_host() ? user.host.get_host() : "");
            }
            user.auth_string.str=
              get_field(&global_acl_memory,
                        table->field[MYSQL_USER_FIELD_AUTHENTICATION_STRING]);
            if (!user.auth_string.str)
              user.auth_string.str= const_cast<char*>("");
            user.auth_string.length= strlen(user.auth_string.str);
          }
          else /* skip auth_string if there's no plugin */
            next_field++;
        }

        if (table->s->fields > MYSQL_USER_FIELD_PASSWORD_EXPIRED)
        {
          char *tmpstr= get_field(&global_acl_memory,
                                  table->field[MYSQL_USER_FIELD_PASSWORD_EXPIRED]);
          if (tmpstr && (*tmpstr == 'Y' || *tmpstr == 'y'))
          {
            user.password_expired= true;

            if (!auth_plugin_supports_expiration(user.plugin.str))
            {
              sql_print_warning("'user' entry '%s@%s' has the password ignore "
                                "flag raised, but its authentication plugin "
                                "doesn't support password expiration. "
                                "The user id will be ignored.",
                                user.user ? user.user : "",
                                user.host.get_host() ? user.host.get_host() : "");
              continue;
            }
            password_expired= true;
          }
        }
      } // end if (table->s->fields >= 31)
      else
      {
        user.ssl_type=SSL_TYPE_NONE;
#ifndef TO_BE_REMOVED
        if (table->s->fields <= 13)
        {						// Without grant
          if (user.access & CREATE_ACL)
            user.access|=REFERENCES_ACL | INDEX_ACL | ALTER_ACL;
        }
        /* Convert old privileges */
        user.access|= LOCK_TABLES_ACL | CREATE_TMP_ACL | SHOW_DB_ACL;
        if (user.access & FILE_ACL)
          user.access|= REPL_CLIENT_ACL | REPL_SLAVE_ACL;
        if (user.access & PROCESS_ACL)
          user.access|= SUPER_ACL | EXECUTE_ACL;
#endif
      }
      if (!user.plugin.length)
      {
        /*
           Set plugin deduced from password length.
           We can reach here in two cases:
           1. mysql.user doesn't have plugin field
           2. Plugin field is empty
         */
        user.plugin= native_password_plugin_name;
        if (password_len == SCRAMBLED_PASSWORD_CHAR_LENGTH_323)
          user.plugin= old_password_plugin_name;
  
      }
      /*
         Transform hex to octets and adjust the format.
       */
      if (set_user_salt(&user, password, password_len))
      {
        sql_print_warning("Found invalid password for user: '%s@%s'; "
                          "Ignoring user", user.user ? user.user : "",
                          user.host.get_host() ? user.host.get_host() : "");
        continue;
      }

      /* set_user_salt resets expiration flag so restore it */
      user.password_expired= password_expired;

      (void) push_dynamic(&acl_users,(uchar*) &user);
      if (user.host.check_allow_all_hosts())
        allow_all_hosts=1;			// Anyone can connect
    }
  } // END while reading records from the mysql.user table
  
  my_qsort((uchar*) dynamic_element(&acl_users,0,ACL_USER*),acl_users.elements,
	   sizeof(ACL_USER),(qsort_cmp) acl_compare);
  end_read_record(&read_record_info);
  freeze_size(&acl_users);

  /* Legacy password integrity checks ----------------------------------------*/
  { 
    password_length= table->field[MYSQL_USER_FIELD_PASSWORD]->field_length /
      table->field[MYSQL_USER_FIELD_PASSWORD]->charset()->mbmaxlen;
    if (password_length < SCRAMBLED_PASSWORD_CHAR_LENGTH_323)
    {
      sql_print_error("Fatal error: mysql.user table is damaged or in "
                      "unsupported 3.20 format.");
      goto end;
    }
  
    DBUG_PRINT("info",("user table fields: %d, password length: %d",
  		     table->s->fields, password_length));

    mysql_mutex_lock(&LOCK_global_system_variables);
    if (password_length < SCRAMBLED_PASSWORD_CHAR_LENGTH)
    { 
      if (opt_secure_auth)
      {
        mysql_mutex_unlock(&LOCK_global_system_variables);
        sql_print_error("Fatal error: mysql.user table is in old format, "
                        "but server started with --secure-auth option.");
        goto end;
      }
      mysql_user_table_is_in_short_password_format= true;
      if (global_system_variables.old_passwords)
        mysql_mutex_unlock(&LOCK_global_system_variables);
      else
      {
        global_system_variables.old_passwords= 1;
        mysql_mutex_unlock(&LOCK_global_system_variables);
        sql_print_warning("mysql.user table is not updated to new password format; "
                          "Disabling new password usage until "
                          "mysql_fix_privilege_tables is run");
      }
      thd->variables.old_passwords= 1;
    }
    else
    {
      mysql_user_table_is_in_short_password_format= false;
      mysql_mutex_unlock(&LOCK_global_system_variables);
    }
  } /* End legacy password integrity checks ----------------------------------*/
  
  /*
    Prepare reading from the mysql.db table
  */
  if (init_read_record(&read_record_info, thd, table=tables[1].table,
                       NULL, 1, 1, FALSE))
    goto end;
  table->use_all_columns();
  while (!(read_record_info.read_record(&read_record_info)))
  {
    /* Reading record in mysql.db */
    ACL_DB db;
    db.host.update_hostname(get_field(&global_acl_memory, 
                            table->field[MYSQL_DB_FIELD_HOST]));
    db.db=get_field(&global_acl_memory, table->field[MYSQL_DB_FIELD_DB]);
    if (!db.db)
    {
      sql_print_warning("Found an entry in the 'db' table with empty database name; Skipped");
      continue;
    }
    db.user=get_field(&global_acl_memory, table->field[MYSQL_DB_FIELD_USER]);
    if (check_no_resolve && hostname_requires_resolving(db.host.get_host()))
    {
      sql_print_warning("'db' entry '%s %s@%s' "
		        "ignored in --skip-name-resolve mode.",
		        db.db,
			db.user ? db.user : "",
			db.host.get_host() ? db.host.get_host() : "");
      continue;
    }
    db.access=get_access(table,3);
    db.access=fix_rights_for_db(db.access);
    if (lower_case_table_names)
    {
      /*
        convert db to lower case and give a warning if the db wasn't
        already in lower case
      */
      (void)strmov(tmp_name, db.db);
      my_casedn_str(files_charset_info, db.db);
      if (strcmp(db.db, tmp_name) != 0)
      {
        sql_print_warning("'db' entry '%s %s@%s' had database in mixed "
                          "case that has been forced to lowercase because "
                          "lower_case_table_names is set. It will not be "
                          "possible to remove this privilege using REVOKE.",
		          db.db,
			  db.user ? db.user : "",
			  db.host.get_host() ? db.host.get_host() : "");
      }
    }
    db.sort=get_sort(3,db.host.get_host(),db.db,db.user);
#ifndef TO_BE_REMOVED
    if (table->s->fields <=  9)
    {						// Without grant
      if (db.access & CREATE_ACL)
	db.access|=REFERENCES_ACL | INDEX_ACL | ALTER_ACL;
    }
#endif
    (void) push_dynamic(&acl_dbs,(uchar*) &db);
  } // END reading records from mysql.db tables
  
  my_qsort((uchar*) dynamic_element(&acl_dbs,0,ACL_DB*),acl_dbs.elements,
	   sizeof(ACL_DB),(qsort_cmp) acl_compare);
  end_read_record(&read_record_info);
  freeze_size(&acl_dbs);

  /* Prepare to read records from the mysql.proxies_priv table */
  if (tables[2].table)
  {
    if (init_read_record(&read_record_info, thd, table= tables[2].table,
                         NULL, 1, 1, FALSE))
      goto end;
    table->use_all_columns();
    while (!(read_record_info.read_record(&read_record_info)))
    {
      /* Reading record in mysql.proxies_priv */
      ACL_PROXY_USER proxy;
      proxy.init(table, &global_acl_memory);
      if (proxy.check_validity(check_no_resolve))
        continue;
      if (push_dynamic(&acl_proxy_users, (uchar*) &proxy))
      {
        end_read_record(&read_record_info);
        goto end;
      }
    } // END reading records from the mysql.proxies_priv table

    my_qsort((uchar*) dynamic_element(&acl_proxy_users, 0, ACL_PROXY_USER*),
             acl_proxy_users.elements,
             sizeof(ACL_PROXY_USER), (qsort_cmp) acl_compare);
    end_read_record(&read_record_info);
  }
  else
  {
    sql_print_error("Missing system table mysql.proxies_priv; "
                    "please run mysql_upgrade to create it");
  }
  freeze_size(&acl_proxy_users);

  validate_user_plugin_records();
  init_check_host();

  initialized=1;
  return_val= FALSE;

end:
  thd->variables.sql_mode= old_sql_mode;
  DBUG_RETURN(return_val);
}