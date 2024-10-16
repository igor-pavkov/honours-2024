slice_unit::~slice_unit()
{
  ctx->nal_parser.free_NAL_unit(nal);

  if (thread_contexts) {
    delete[] thread_contexts;
  }
}