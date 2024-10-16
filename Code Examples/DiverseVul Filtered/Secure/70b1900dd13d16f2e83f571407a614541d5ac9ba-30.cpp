static struct getout *new_getout(struct Configurable *config)
{
  struct getout *node =malloc(sizeof(struct getout));
  struct getout *last= config->url_last;
  if(node) {
    /* clear the struct */
    memset(node, 0, sizeof(struct getout));

    /* append this new node last in the list */
    if(last)
      last->next = node;
    else
      config->url_list = node; /* first node */

    /* move the last pointer */
    config->url_last = node;
  }
  return node;
}