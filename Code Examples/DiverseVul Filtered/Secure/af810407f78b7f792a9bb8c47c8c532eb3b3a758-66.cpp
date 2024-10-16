static bool update_frm_version(TABLE *table)
{
  char path[FN_REFLEN];
  File file;
  int result= 1;
  DBUG_ENTER("update_frm_version");

  /*
    No need to update frm version in case table was created or checked
    by server with the same version. This also ensures that we do not
    update frm version for temporary tables as this code doesn't support
    temporary tables.
  */
  if (table->s->mysql_version == MYSQL_VERSION_ID)
    DBUG_RETURN(0);

  strxmov(path, table->s->normalized_path.str, reg_ext, NullS);

  if ((file= mysql_file_open(key_file_frm,
                             path, O_RDWR|O_BINARY, MYF(MY_WME))) >= 0)
  {
    uchar version[4];

    int4store(version, MYSQL_VERSION_ID);

    if ((result= (int)mysql_file_pwrite(file, (uchar*) version, 4, 51L,
                                        MYF(MY_WME+MY_NABP))))
      goto err;

    table->s->mysql_version= MYSQL_VERSION_ID;
  }
err:
  if (file >= 0)
    (void) mysql_file_close(file, MYF(MY_WME));
  DBUG_RETURN(result);
}