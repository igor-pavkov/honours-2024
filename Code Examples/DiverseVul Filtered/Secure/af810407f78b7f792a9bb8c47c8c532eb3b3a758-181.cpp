void handler::column_bitmaps_signal()
{
  DBUG_ENTER("column_bitmaps_signal");
  if (table)
    DBUG_PRINT("info", ("read_set: %p  write_set: %p",
                        table->read_set, table->write_set));
  DBUG_VOID_RETURN;
}