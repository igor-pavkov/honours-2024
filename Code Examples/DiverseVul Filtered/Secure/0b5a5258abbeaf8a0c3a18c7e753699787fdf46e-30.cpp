Event_basic::Event_basic()
{
  DBUG_ENTER("Event_basic::Event_basic");
  /* init memory root */
  init_sql_alloc(&mem_root, 256, 512, MYF(0));
  dbname.str= name.str= NULL;
  dbname.length= name.length= 0;
  time_zone= NULL;
  DBUG_VOID_RETURN;
}