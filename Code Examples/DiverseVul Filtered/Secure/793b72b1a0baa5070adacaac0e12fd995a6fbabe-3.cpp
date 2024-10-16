int mysql_db_async_result(SV* h, MYSQL_RES** resp)
{
  dTHX;
  D_imp_xxh(h);
  imp_dbh_t* dbh;
  MYSQL* svsock = NULL;
  MYSQL_RES* _res;
  int retval = 0;
  int htype;

  if(! resp) {
      resp = &_res;
  }
  htype = DBIc_TYPE(imp_xxh);


  if(htype == DBIt_DB) {
      D_imp_dbh(h);
      dbh = imp_dbh;
  } else {
      D_imp_sth(h);
      D_imp_dbh_from_sth;
      dbh = imp_dbh;
  }

  if(! dbh->async_query_in_flight) {
      do_error(h, 2000, "Gathering asynchronous results for a synchronous handle", "HY000");
      return -1;
  }
  if(dbh->async_query_in_flight != imp_xxh) {
      do_error(h, 2000, "Gathering async_query_in_flight results for the wrong handle", "HY000");
      return -1;
  }
  dbh->async_query_in_flight = NULL;

  svsock= dbh->pmysql;
  retval= mysql_read_query_result(svsock);
  if(! retval) {
    *resp= mysql_store_result(svsock);

    if (mysql_errno(svsock))
      do_error(h, mysql_errno(svsock), mysql_error(svsock), mysql_sqlstate(svsock));
    if (!*resp)
      retval= mysql_affected_rows(svsock);
    else {
      retval= mysql_num_rows(*resp);
      if(resp == &_res) {
        mysql_free_result(*resp);
      }
    }
    if(htype == DBIt_ST) {
      D_imp_sth(h);
      D_imp_dbh_from_sth;

      if((my_ulonglong)retval+1 != (my_ulonglong)-1) {
        if(! *resp) {
          imp_sth->insertid= mysql_insert_id(svsock);
#if MYSQL_VERSION_ID >= MULTIPLE_RESULT_SET_VERSION
          if(! mysql_more_results(svsock))
            DBIc_ACTIVE_off(imp_sth);
#endif
        } else {
          DBIc_NUM_FIELDS(imp_sth)= mysql_num_fields(imp_sth->result);
          imp_sth->done_desc= 0;
          imp_sth->fetch_done= 0;
        }
      }
      imp_sth->warning_count = mysql_warning_count(imp_dbh->pmysql);
    }
  } else {
     do_error(h, mysql_errno(svsock), mysql_error(svsock),
              mysql_sqlstate(svsock));
     return -1;
  }
 return retval;
}