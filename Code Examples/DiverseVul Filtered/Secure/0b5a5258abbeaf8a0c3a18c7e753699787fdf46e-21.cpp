Event_timed::Event_timed():
  created(0), modified(0), sql_mode(0)
{
  DBUG_ENTER("Event_timed::Event_timed");
  init();
  DBUG_VOID_RETURN;
}