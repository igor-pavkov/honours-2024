static void AddAttrToList( AttVal** list, AttVal* av )
{
  if ( *list == NULL )
    *list = av;
  else
  {
    AttVal* here = *list;
    while ( here->next )
      here = here->next;
    here->next = av;
  }
}