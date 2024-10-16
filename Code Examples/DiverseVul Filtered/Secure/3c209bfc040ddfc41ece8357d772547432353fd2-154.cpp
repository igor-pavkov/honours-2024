Item_subselect::~Item_subselect()
{
  DBUG_ENTER("Item_subselect::~Item_subselect");
  DBUG_PRINT("enter", ("this: %p", this));
  if (own_engine)
    delete engine;
  else
    engine->cleanup();
  engine= NULL;
  DBUG_VOID_RETURN;
}