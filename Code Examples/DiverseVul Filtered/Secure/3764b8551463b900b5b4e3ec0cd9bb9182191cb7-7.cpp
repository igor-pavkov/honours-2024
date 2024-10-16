static long get_content_length (orderedmap hashofheaders)
{
        char *data;
        long content_length = -1;

        data = orderedmap_find (hashofheaders, "content-length");

        if (data)
                content_length = atol (data);

        return content_length;
}