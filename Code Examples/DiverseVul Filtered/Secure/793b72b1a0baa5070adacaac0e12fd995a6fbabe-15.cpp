SV* dbd_db_quote(SV *dbh, SV *str, SV *type)
{
  dTHX;
  SV *result;

  if (SvGMAGICAL(str))
    mg_get(str);

  if (!SvOK(str))
    result= newSVpvn("NULL", 4);
  else
  {
    char *ptr, *sptr;
    STRLEN len;

    D_imp_dbh(dbh);

    if (type && SvMAGICAL(type))
      mg_get(type);

    if (type  &&  SvOK(type))
    {
      int i;
      int tp= SvIV(type);
      for (i= 0;  i < (int)SQL_GET_TYPE_INFO_num;  i++)
      {
        const sql_type_info_t *t= &SQL_GET_TYPE_INFO_values[i];
        if (t->data_type == tp)
        {
          if (!t->literal_prefix)
            return Nullsv;
          break;
        }
      }
    }

    ptr= SvPV(str, len);
    result= newSV(len*2+3);
#ifdef SvUTF8
    if (SvUTF8(str)) SvUTF8_on(result);
#endif
    sptr= SvPVX(result);

    *sptr++ = '\'';
    sptr+= mysql_real_escape_string(imp_dbh->pmysql, sptr,
                                     ptr, len);
    *sptr++= '\'';
    SvPOK_on(result);
    SvCUR_set(result, sptr - SvPVX(result));
    /* Never hurts NUL terminating a Per string */
    *sptr++= '\0';
  }
  return result;
}