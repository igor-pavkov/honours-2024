Ordered_key::~Ordered_key()
{
  my_free(key_buff);
  my_bitmap_free(&null_key);
}