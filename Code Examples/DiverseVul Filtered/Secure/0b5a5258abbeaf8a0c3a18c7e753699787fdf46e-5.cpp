Event_queue_element::mark_last_executed(THD *thd)
{
  last_executed= thd->query_start();

  execution_count++;
}