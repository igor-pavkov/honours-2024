static void clean_getout(struct Configurable *config)
{
  struct getout *node=config->url_list;
  struct getout *next;

  while(node) {
    next = node->next;
    if(node->url)
      free(node->url);
    if(node->outfile)
      free(node->outfile);
    if(node->infile)
      free(node->infile);
    free(node);

    node = next; /* GOTO next */
  }
}