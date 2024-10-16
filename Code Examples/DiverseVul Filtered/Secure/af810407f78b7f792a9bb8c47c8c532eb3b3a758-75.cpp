static void update_discovery_counters(handlerton *hton, int val)
{
  if (hton->discover_table_existence == full_discover_for_existence)
    need_full_discover_for_existence+= val;

  if (hton->discover_table_names && hton->tablefile_extensions[0])
    engines_with_discover_file_names+= val;

  if (hton->discover_table)
    engines_with_discover+= val;
}