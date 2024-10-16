static my_bool cli_read_query_result(MYSQL *mysql)
{
  uchar *pos;
  ulong field_count;
  MYSQL_DATA *fields;
  ulong length;
  DBUG_ENTER("cli_read_query_result");

  if ((length = cli_safe_read(mysql)) == packet_error)
    DBUG_RETURN(1);
  free_old_query(mysql);		/* Free old result */
#ifdef MYSQL_CLIENT			/* Avoid warn of unused labels*/
get_info:
#endif
  pos=(uchar*) mysql->net.read_pos;
  if ((field_count= net_field_length(&pos)) == 0)
  {
    mysql->affected_rows= net_field_length_ll(&pos);
    mysql->insert_id=	  net_field_length_ll(&pos);
    DBUG_PRINT("info",("affected_rows: %lu  insert_id: %lu",
		       (ulong) mysql->affected_rows,
		       (ulong) mysql->insert_id));
    if (protocol_41(mysql))
    {
      mysql->server_status=uint2korr(pos); pos+=2;
      mysql->warning_count=uint2korr(pos); pos+=2;
    }
    else if (mysql->server_capabilities & CLIENT_TRANSACTIONS)
    {
      /* MySQL 4.0 protocol */
      mysql->server_status=uint2korr(pos); pos+=2;
      mysql->warning_count= 0;
    }
    DBUG_PRINT("info",("status: %u  warning_count: %u",
		       mysql->server_status, mysql->warning_count));
    if (pos < mysql->net.read_pos+length && net_field_length(&pos))
      mysql->info=(char*) pos;
    DBUG_RETURN(0);
  }
#ifdef MYSQL_CLIENT
  if (field_count == NULL_LENGTH)		/* LOAD DATA LOCAL INFILE */
  {
    int error;

    if (!(mysql->options.client_flag & CLIENT_LOCAL_FILES))
    {
      set_mysql_error(mysql, CR_MALFORMED_PACKET, unknown_sqlstate);
      DBUG_RETURN(1);
    }   

    error= handle_local_infile(mysql,(char*) pos);
    if ((length= cli_safe_read(mysql)) == packet_error || error)
      DBUG_RETURN(1);
    goto get_info;				/* Get info packet */
  }
#endif
  if (!(mysql->server_status & SERVER_STATUS_AUTOCOMMIT))
    mysql->server_status|= SERVER_STATUS_IN_TRANS;

  if (!(fields=cli_read_rows(mysql,(MYSQL_FIELD*)0, protocol_41(mysql) ? 7:5)))
    DBUG_RETURN(1);
  if (!(mysql->fields=unpack_fields(mysql, fields,&mysql->field_alloc,
				    (uint) field_count,0,
				    mysql->server_capabilities)))
    DBUG_RETURN(1);
  mysql->status= MYSQL_STATUS_GET_RESULT;
  mysql->field_count= (uint) field_count;
  DBUG_PRINT("exit",("ok"));
  DBUG_RETURN(0);
}