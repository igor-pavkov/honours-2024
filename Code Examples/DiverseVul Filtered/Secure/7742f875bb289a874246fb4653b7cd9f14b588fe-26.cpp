select_incremental_lsn_from_history(lsn_t *incremental_lsn)
{
	MYSQL_RES *mysql_result;
	MYSQL_ROW row;
	char query[1000];
	char buf[100];

	if (opt_incremental_history_name) {
		mysql_real_escape_string(mysql_connection, buf,
				opt_incremental_history_name,
				strlen(opt_incremental_history_name));
		ut_snprintf(query, sizeof(query),
			"SELECT innodb_to_lsn "
			"FROM PERCONA_SCHEMA.xtrabackup_history "
			"WHERE name = '%s' "
			"AND innodb_to_lsn IS NOT NULL "
			"ORDER BY innodb_to_lsn DESC LIMIT 1",
			buf);
	}

	if (opt_incremental_history_uuid) {
		mysql_real_escape_string(mysql_connection, buf,
				opt_incremental_history_uuid,
				strlen(opt_incremental_history_uuid));
		ut_snprintf(query, sizeof(query),
			"SELECT innodb_to_lsn "
			"FROM PERCONA_SCHEMA.xtrabackup_history "
			"WHERE uuid = '%s' "
			"AND innodb_to_lsn IS NOT NULL "
			"ORDER BY innodb_to_lsn DESC LIMIT 1",
			buf);
	}

	mysql_result = xb_mysql_query(mysql_connection, query, true);

	ut_ad(mysql_num_fields(mysql_result) == 1);
	if (!(row = mysql_fetch_row(mysql_result))) {
		msg("Error while attempting to find history record "
			"for %s %s\n",
			opt_incremental_history_uuid ? "uuid" : "name",
			opt_incremental_history_uuid ?
		    		opt_incremental_history_uuid :
		    		opt_incremental_history_name);
		return(false);
	}

	*incremental_lsn = strtoull(row[0], NULL, 10);

	mysql_free_result(mysql_result);

	msg("Found and using lsn: " LSN_PF " for %s %s\n", *incremental_lsn,
		opt_incremental_history_uuid ? "uuid" : "name",
		opt_incremental_history_uuid ?
	    		opt_incremental_history_uuid :
	    		opt_incremental_history_name);

	return(true);
}