static struct Curl_easy* gethandleathead(struct curl_llist *pipeline)
{
  struct curl_llist_element *curr = pipeline->head;
#ifdef DEBUGBUILD
  {
    struct curl_llist_element *p = pipeline->head;
    while(p) {
      struct Curl_easy *e = p->ptr;
      DEBUGASSERT(GOOD_EASY_HANDLE(e));
      p = p->next;
    }
  }
#endif
  if(curr) {
    return (struct Curl_easy *) curr->ptr;
  }

  return NULL;
}