int blosc2_getitem_ctx(blosc2_context* context, const void* src, int32_t srcsize,
    int start, int nitems, void* dest) {
  uint8_t* _src = (uint8_t*)(src);
  int result;

  /* Minimally populate the context */
  context->typesize = (uint8_t)_src[3];
  context->blocksize = sw32_(_src + 8);
  context->header_flags = *(_src + 2);
  context->filter_flags = get_filter_flags(*(_src + 2), context->typesize);
  if (context->serial_context == NULL) {
    context->serial_context = create_thread_context(context, 0);
  }

  /* Call the actual getitem function */
  result = _blosc_getitem(context, src, srcsize, start, nitems, dest);

  return result;
}