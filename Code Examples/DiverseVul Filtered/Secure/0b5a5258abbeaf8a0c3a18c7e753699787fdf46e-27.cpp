Event_queue_element_for_exec::init(LEX_STRING db, LEX_STRING n)
{
  if (!(dbname.str= my_strndup(db.str, dbname.length= db.length, MYF(MY_WME))))
    return TRUE;
  if (!(name.str= my_strndup(n.str, name.length= n.length, MYF(MY_WME))))
  {
    my_free(dbname.str);
    return TRUE;
  }
  return FALSE;
}