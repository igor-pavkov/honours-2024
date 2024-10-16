Event_timed::init()
{
  DBUG_ENTER("Event_timed::init");

  definer_user.str= definer_host.str= body.str= comment.str= NULL;
  definer_user.length= definer_host.length= body.length= comment.length= 0;

  sql_mode= 0;

  DBUG_VOID_RETURN;
}