static void FreeMultiInfo (struct multi_files *multi_start)
{
  struct multi_files *multi;
  while (multi_start) {
    multi = multi_start;
    multi_start = multi_start->next;
    free (multi);
  }
}