int dbd_db_disconnect(SV* dbh, imp_dbh_t* imp_dbh)
{
#ifdef dTHR
  dTHR;
#endif
  dTHX;
  D_imp_xxh(dbh);

  /* We assume that disconnect will always work       */
  /* since most errors imply already disconnected.    */
  DBIc_ACTIVE_off(imp_dbh);
  if (DBIc_TRACE_LEVEL(imp_xxh) >= 2)
    PerlIO_printf(DBIc_LOGPIO(imp_xxh), "imp_dbh->pmysql: %p\n",
		              imp_dbh->pmysql);
  mysql_close(imp_dbh->pmysql );

  /* We don't free imp_dbh since a reference still exists    */
  /* The DESTROY method is the only one to 'free' memory.    */
  return TRUE;
}