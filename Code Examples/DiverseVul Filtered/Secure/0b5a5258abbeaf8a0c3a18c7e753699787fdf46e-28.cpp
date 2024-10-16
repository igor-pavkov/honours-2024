Event_queue_element::Event_queue_element():
  on_completion(Event_parse_data::ON_COMPLETION_DROP),
  status(Event_parse_data::ENABLED), expression(0), dropped(FALSE),
  execution_count(0)
{
  DBUG_ENTER("Event_queue_element::Event_queue_element");

  starts= ends= execute_at= last_executed= 0;
  starts_null= ends_null= execute_at_null= TRUE;

  DBUG_VOID_RETURN;
}