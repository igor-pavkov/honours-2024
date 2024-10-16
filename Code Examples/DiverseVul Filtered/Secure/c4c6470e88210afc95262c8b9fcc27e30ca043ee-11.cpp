void blosc_set_schunk(blosc2_schunk* schunk) {
  g_schunk = schunk;
  g_global_context->schunk = schunk;
}