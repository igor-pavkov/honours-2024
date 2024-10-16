extern void onig_add_end_call(void (*func)(void))
{
  OnigEndCallListItemType* item;

  item = (OnigEndCallListItemType* )xmalloc(sizeof(*item));
  if (item == 0) return ;

  item->next = EndCallTop;
  item->func = func;

  EndCallTop = item;
}