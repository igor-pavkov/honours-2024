Event_basic::~Event_basic()
{
  DBUG_ENTER("Event_basic::~Event_basic");
  free_root(&mem_root, MYF(0));
  DBUG_VOID_RETURN;
}