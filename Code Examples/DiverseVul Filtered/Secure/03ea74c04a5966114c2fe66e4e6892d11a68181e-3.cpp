static size_t handle_returned_data (void *ptr, size_t size, size_t nmemb, void *stream)
{
    return size * nmemb;
}