void buffer_delete(struct buffer_st *b)
{
  efree(b->data);
  b->length = 0;
  b->offset = 0;
  b->ptr = NULL;
  b->data = NULL;
}