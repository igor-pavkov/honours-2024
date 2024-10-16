slice_unit::slice_unit(decoder_context* decctx)
  : nal(NULL),
    shdr(NULL),
    imgunit(NULL),
    flush_reorder_buffer(false),
    nThreads(0),
    first_decoded_CTB_RS(-1),
    last_decoded_CTB_RS(-1),
    thread_contexts(NULL),
    ctx(decctx)
{
  state = Unprocessed;
  nThreadContexts = 0;
}