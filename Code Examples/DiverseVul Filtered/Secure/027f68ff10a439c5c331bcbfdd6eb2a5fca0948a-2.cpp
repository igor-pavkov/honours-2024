void buffer_add(struct buffer_st *b, char c)
{
  if ((INT_MAX - b->length) <= 512) {
		return;
  }
  *(b->ptr++) = c;
  b->offset++;
  if (b->offset == b->length) {
    b->length += 512;
    b->data = erealloc(b->data, b->length);
    b->ptr = b->data + b->offset;
  }
}