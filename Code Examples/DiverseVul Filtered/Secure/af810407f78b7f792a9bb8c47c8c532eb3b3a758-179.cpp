void handler::drop_table(const char *name)
{
  ha_close();
  delete_table(name);
}