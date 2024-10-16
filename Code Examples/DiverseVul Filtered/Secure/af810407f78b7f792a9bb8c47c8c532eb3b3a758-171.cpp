int handler::ha_end_bulk_insert()
{
  DBUG_ENTER("handler::ha_end_bulk_insert");
  DBUG_EXECUTE_IF("crash_end_bulk_insert",
                  { extra(HA_EXTRA_FLUSH) ; DBUG_SUICIDE();});
  estimation_rows_to_insert= 0;
  DBUG_RETURN(end_bulk_insert());
}