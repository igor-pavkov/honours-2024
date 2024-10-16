void Discovered_table_list::remove_duplicates()
{
  LEX_CSTRING **src= tables->front();
  LEX_CSTRING **dst= src;
  sort();
  while (++dst <= tables->back())
  {
    LEX_CSTRING *s= *src, *d= *dst;
    DBUG_ASSERT(strncmp(s->str, d->str, MY_MIN(s->length, d->length)) <= 0);
    if ((s->length != d->length || strncmp(s->str, d->str, d->length)))
    {
      src++;
      if (src != dst)
        *src= *dst;
    }
  }
  tables->elements(src - tables->front() + 1);
}