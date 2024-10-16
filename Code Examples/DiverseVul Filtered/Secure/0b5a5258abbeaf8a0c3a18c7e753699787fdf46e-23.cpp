Event_queue_element_for_exec::~Event_queue_element_for_exec()
{
  my_free(dbname.str);
  my_free(name.str);
}