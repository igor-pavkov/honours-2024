int is_filtered_essid(unsigned char *essid)
{
    int ret = 0;
    int i;

    if(G.f_essid)
    {
        for(i=0; i<G.f_essid_count; i++)
        {
            if(strncmp((char*)essid, G.f_essid[i], MAX_IE_ELEMENT_SIZE) == 0)
            {
                return 0;
            }
        }

        ret = 1;
    }

#ifdef HAVE_PCRE
    if(G.f_essid_regex)
    {
        return pcre_exec(G.f_essid_regex, NULL, (char*)essid, strnlen((char *)essid, MAX_IE_ELEMENT_SIZE), 0, 0, NULL, 0) < 0;
    }
#endif

    return ret;
}