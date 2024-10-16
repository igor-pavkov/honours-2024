int dbd_st_more_results(SV* sth, imp_sth_t* imp_sth)
{
  dTHX;
  D_imp_dbh_from_sth;
  D_imp_xxh(sth);

  int use_mysql_use_result=imp_sth->use_mysql_use_result;
  int next_result_return_code, i;
  MYSQL* svsock= imp_dbh->pmysql;

  if (!SvROK(sth) || SvTYPE(SvRV(sth)) != SVt_PVHV)
    croak("Expected hash array");

  if (!mysql_more_results(svsock))
  {
    /* No more pending result set(s)*/
    if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
      PerlIO_printf(DBIc_LOGPIO(imp_xxh),
		    "\n      <- dbs_st_more_results no more results\n");
    return 0;
  }

  if (imp_sth->use_server_side_prepare)
  {
    do_warn(sth, JW_ERR_NOT_IMPLEMENTED,
            "Processing of multiple result set is not possible with server side prepare");
    return 0;
  }

  /*
   *  Free cached array attributes
   */
  for (i= 0; i < AV_ATTRIB_LAST;  i++)
  {
    if (imp_sth->av_attr[i])
      SvREFCNT_dec(imp_sth->av_attr[i]);

    imp_sth->av_attr[i]= Nullav;
  }

  /* Release previous MySQL result*/
  if (imp_sth->result)
    mysql_free_result(imp_sth->result);

  if (DBIc_ACTIVE(imp_sth))
    DBIc_ACTIVE_off(imp_sth);

  next_result_return_code= mysql_next_result(svsock);

  imp_sth->warning_count = mysql_warning_count(imp_dbh->pmysql);

  /*
    mysql_next_result returns
      0 if there are more results
     -1 if there are no more results
     >0 if there was an error
   */
  if (next_result_return_code > 0)
  {
    do_error(sth, mysql_errno(svsock), mysql_error(svsock),
             mysql_sqlstate(svsock));

    return 0;
  }
  else if(next_result_return_code == -1)                                                                                                                  
  {                                                                                                                                                       
    return 0;                                                                                                                                             
  }  
  else
  {
    /* Store the result from the Query */
    imp_sth->result = use_mysql_use_result ?
     mysql_use_result(svsock) : mysql_store_result(svsock);

    if (mysql_errno(svsock))
    {
      do_error(sth, mysql_errno(svsock), mysql_error(svsock), 
               mysql_sqlstate(svsock));
      return 0;
    }

    imp_sth->row_num= mysql_affected_rows(imp_dbh->pmysql);

    if (imp_sth->result == NULL)
    {
      /* No "real" rowset*/
      DBIc_NUM_FIELDS(imp_sth)= 0; /* for DBI <= 1.53 */
      DBIS->set_attr_k(sth, sv_2mortal(newSVpvn("NUM_OF_FIELDS",13)), 0,
			               sv_2mortal(newSViv(0)));
      return 1;
    }
    else
    {
      /* We have a new rowset */
      imp_sth->currow=0;


      /* delete cached handle attributes */
      /* XXX should be driven by a list to ease maintenance */
      (void)hv_delete((HV*)SvRV(sth), "NAME", 4, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "NULLABLE", 8, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "NUM_OF_FIELDS", 13, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "PRECISION", 9, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "SCALE", 5, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "TYPE", 4, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_insertid", 14, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_is_auto_increment", 23, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_is_blob", 13, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_is_key", 12, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_is_num", 12, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_is_pri_key", 16, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_length", 12, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_max_length", 16, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_table", 11, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_type", 10, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_type_name", 15, G_DISCARD);
      (void)hv_delete((HV*)SvRV(sth), "mysql_warning_count", 20, G_DISCARD);

      /* Adjust NUM_OF_FIELDS - which also adjusts the row buffer size */
      DBIc_NUM_FIELDS(imp_sth)= 0; /* for DBI <= 1.53 */
      DBIc_DBISTATE(imp_sth)->set_attr_k(sth, sv_2mortal(newSVpvn("NUM_OF_FIELDS",13)), 0,
          sv_2mortal(newSViv(mysql_num_fields(imp_sth->result)))
      );

      DBIc_ACTIVE_on(imp_sth);

      imp_sth->done_desc = 0;
    }
    imp_dbh->pmysql->net.last_errno= 0;
    return 1;
  }
}