static void curl_clean_state(CURLState *s)
{
    if (s->s->multi)
        curl_multi_remove_handle(s->s->multi, s->curl);
    s->in_use = 0;
}