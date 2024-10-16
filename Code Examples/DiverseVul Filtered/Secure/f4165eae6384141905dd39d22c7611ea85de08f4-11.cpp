static uchar* acl_entry_get_key(acl_entry *entry, size_t *length,
                                my_bool not_used MY_ATTRIBUTE((unused)))
{
  *length=(uint) entry->length;
  return (uchar*) entry->key;
}