void blosc_set_threads_callback(blosc_threads_callback callback, void *callback_data)
{
  threads_callback = callback;
  threads_callback_data = callback_data;
}