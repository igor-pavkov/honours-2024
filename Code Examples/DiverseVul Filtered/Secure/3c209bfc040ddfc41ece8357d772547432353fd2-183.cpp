void Item_subselect::cleanup()
{
  DBUG_ENTER("Item_subselect::cleanup");
  Item_result_field::cleanup();
  if (old_engine)
  {
    if (engine)
      engine->cleanup();
    engine= old_engine;
    old_engine= 0;
  }
  if (engine)
    engine->cleanup();
  reset();
  filesort_buffer.free_sort_buffer();
  my_free(sortbuffer.str);
  sortbuffer= null_lex_str;

  value_assigned= 0;
  expr_cache= 0;
  forced_const= FALSE;
  DBUG_PRINT("info", ("exec_counter: %d", exec_counter));
#ifndef DBUG_OFF
  exec_counter= 0;
#endif
  DBUG_VOID_RETURN;
}