void buffer_new(struct buffer_st *b)
{
  b->length = 512;
  b->data = emalloc(sizeof(char)*(b->length));
  b->data[0] = 0;
  b->ptr = b->data;
  b->offset = 0;
}