static int sql_auxprop_lookup(void *glob_context,
			       sasl_server_params_t *sparams,
			       unsigned flags,
			       const char *user,
			       unsigned ulen) 
{
    char *userid = NULL;
    /* realm could be used for something clever */
    char *realm = NULL;
    const char *user_realm = NULL;
    const struct propval *to_fetch, *cur;
    char value[8192];
    size_t value_len;
    char *user_buf;
    char *query = NULL;
    char *escap_userid = NULL;
    char *escap_realm = NULL;
    sql_settings_t *settings;
    int verify_against_hashed_password;
    int saw_user_password = 0;
    void *conn = NULL;
    int do_txn = 0;
    int ret;
    
    if (!glob_context || !sparams || !user) return SASL_BADPARAM;
    
    /* setup the settings */
    settings = (sql_settings_t *) glob_context;
    
    sparams->utils->log(sparams->utils->conn, SASL_LOG_DEBUG,
			"sql plugin Parse the username %s\n", user);
    
    user_buf = sparams->utils->malloc(ulen + 1);
    if (!user_buf) {
	ret = SASL_NOMEM;
	goto done;
    }
    
    memcpy(user_buf, user, ulen);
    user_buf[ulen] = '\0';
    
    if(sparams->user_realm) {
	user_realm = sparams->user_realm;
    } else {
	user_realm = sparams->serverFQDN;
    }
    
    if ((ret = _plug_parseuser(sparams->utils,
			       &userid,
			       &realm,
			       user_realm,
			       sparams->serverFQDN,
			       user_buf)) != SASL_OK ) {
	goto done;
    }
    
    /* just need to escape userid and realm now */
    /* allocate some memory */
    escap_userid = (char *)sparams->utils->malloc(strlen(userid)*2+1);
    escap_realm = (char *)sparams->utils->malloc(strlen(realm)*2+1);
    
    if (!escap_userid || !escap_realm) {
	ret = SASL_NOMEM;
	goto done;
    }
    
    /*************************************/
    
    /* find out what we need to get */
    /* this corrupts const char *user */
    to_fetch = sparams->utils->prop_get(sparams->propctx);
    if (!to_fetch) {
	ret = SASL_NOMEM;
	goto done;
    }

    conn = sql_connect(settings, sparams->utils);
    if (!conn) {
	sparams->utils->log(sparams->utils->conn, SASL_LOG_ERR,
			    "sql plugin couldn't connect to any host\n");
	/* TODO: in the future we might want to extend the internal
	   SQL driver API to return a more detailed error */
	ret = SASL_FAIL;
	goto done;
    }
    
    /* escape out */
    settings->sql_engine->sql_escape_str(escap_userid, userid);
    settings->sql_engine->sql_escape_str(escap_realm, realm);

    verify_against_hashed_password = flags & SASL_AUXPROP_VERIFY_AGAINST_HASH;

    /* Assume that nothing is found */
    ret = SASL_NOUSER;
    for (cur = to_fetch; cur->name; cur++) {
	char *realname = (char *) cur->name;

	/* Only look up properties that apply to this lookup! */
	if (cur->name[0] == '*'
	    && (flags & SASL_AUXPROP_AUTHZID))
	    continue;
	if (!(flags & SASL_AUXPROP_AUTHZID)) {
	    if(cur->name[0] != '*')
		continue;
	    else
		realname = (char*)cur->name + 1;
	}
	
	/* If it's there already, we want to see if it needs to be
	 * overridden. userPassword is a special case, because it's value
	   is always present if SASL_AUXPROP_VERIFY_AGAINST_HASH is specified.
	   When SASL_AUXPROP_VERIFY_AGAINST_HASH is set, we just clear userPassword. */
	if (cur->values && !(flags & SASL_AUXPROP_OVERRIDE) &&
	    (verify_against_hashed_password == 0 ||
	     strcasecmp(realname, SASL_AUX_PASSWORD_PROP) != 0)) {
	    continue;
	} else if (cur->values) {
	    sparams->utils->prop_erase(sparams->propctx, cur->name);
	}

	if (strcasecmp(realname, SASL_AUX_PASSWORD_PROP) == 0) {
	    saw_user_password = 1;
	}

	if (!do_txn) {
	    do_txn = 1;
	    sparams->utils->log(sparams->utils->conn, SASL_LOG_DEBUG,
				"begin transaction");
	    if (settings->sql_engine->sql_begin_txn(conn, sparams->utils)) {
		sparams->utils->log(sparams->utils->conn, SASL_LOG_ERR,
				    "Unable to begin transaction\n");
	    }
	}
    
	sparams->utils->log(sparams->utils->conn, SASL_LOG_DEBUG,
			    "sql plugin create statement from %s %s %s\n",
			    realname, escap_userid, escap_realm);
	
	/* create a statement that we will use */
	query = sql_create_statement(settings->sql_select,
				     realname,escap_userid,
				     escap_realm, NULL,
				     sparams->utils);
	if (query == NULL) {
	    ret = SASL_NOMEM;
	    break;
	}
	
	sparams->utils->log(sparams->utils->conn, SASL_LOG_DEBUG,
			    "sql plugin doing query %s\n", query);
	
	value[0] = '\0';
	value_len = 0;
	/* run the query */
	if (!settings->sql_engine->sql_exec(conn, query, value, sizeof(value),
					    &value_len, sparams->utils)) {
	    sparams->utils->prop_set(sparams->propctx,
				     cur->name,
				     value,
				     (int)value_len);
	    ret = SASL_OK;
	}
	
	sparams->utils->free(query);
    }

    if (flags & SASL_AUXPROP_AUTHZID) {
	/* This is a lie, but the caller can't handle
	   when we return SASL_NOUSER for authorization identity lookup. */
	if (ret == SASL_NOUSER) {
	    ret = SASL_OK;
	}
    } else {
	if (ret == SASL_NOUSER && saw_user_password == 0) {
	    /* Verify user existence by checking presence of
	       the userPassword attribute */
	    if (!do_txn) {
		do_txn = 1;
		sparams->utils->log(sparams->utils->conn, SASL_LOG_DEBUG,
				    "begin transaction");
		if (settings->sql_engine->sql_begin_txn(conn, sparams->utils)) {
		    sparams->utils->log(sparams->utils->conn, SASL_LOG_ERR,
					"Unable to begin transaction\n");
		}
	    }

	    sparams->utils->log(sparams->utils->conn, SASL_LOG_DEBUG,
				"sql plugin create statement from %s %s %s\n",
				SASL_AUX_PASSWORD_PROP,
				escap_userid,
				escap_realm);
    	
	    /* create a statement that we will use */
	    query = sql_create_statement(settings->sql_select,
					 SASL_AUX_PASSWORD_PROP,
					 escap_userid,
					 escap_realm,
					 NULL,
					 sparams->utils);
	    if (query == NULL) {
		ret = SASL_NOMEM;
	    } else {
		sparams->utils->log(sparams->utils->conn, SASL_LOG_DEBUG,
				    "sql plugin doing query %s\n", query);
        	
		value[0] = '\0';
		value_len = 0;
		/* run the query */
		if (!settings->sql_engine->sql_exec(conn,
						    query,
						    value,
						    sizeof(value),
						    &value_len,
						    sparams->utils)) {
		    ret = SASL_OK;
		}
        	
		sparams->utils->free(query);
	    }
	}
    }


    if (do_txn) {
	sparams->utils->log(sparams->utils->conn, SASL_LOG_DEBUG,
			    "commit transaction");
	if (settings->sql_engine->sql_commit_txn(conn, sparams->utils)) {
	    sparams->utils->log(sparams->utils->conn, SASL_LOG_ERR,
				"Unable to commit transaction\n");
	    /* Failure of the commit is non fatal when reading values */
	}
    }
    
  done:
    if (escap_userid) sparams->utils->free(escap_userid);
    if (escap_realm) sparams->utils->free(escap_realm);
    if (conn) settings->sql_engine->sql_close(conn);
    if (userid) sparams->utils->free(userid);
    if (realm) sparams->utils->free(realm);
    if (user_buf) sparams->utils->free(user_buf);

    return (ret);
}