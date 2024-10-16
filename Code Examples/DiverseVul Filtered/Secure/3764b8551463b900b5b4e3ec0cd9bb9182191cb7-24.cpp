static int is_chunked_transfer (orderedmap hashofheaders)
{
        char *data;
        data = orderedmap_find (hashofheaders, "transfer-encoding");
        return data ? !strcmp (data, "chunked") : 0;
}