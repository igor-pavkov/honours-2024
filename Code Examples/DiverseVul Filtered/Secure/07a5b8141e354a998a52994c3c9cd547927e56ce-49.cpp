static char *list_digest_names(void)
{
    char *list = NULL;

#ifdef OPENSSL
# ifdef HAVE_EVP_MD_DO_ALL_SORTED
    struct hstate hstate = { NULL, NULL, K_PER_LINE+1 };

    hstate.seen = (const char **) emalloc_zero(1*sizeof( const char * )); // replaces -> calloc(1, sizeof( const char * ));

    INIT_SSL();
    EVP_MD_do_all_sorted(list_md_fn, &hstate);
    list = hstate.list;
    free(hstate.seen);
# else
    list = (char *)emalloc(sizeof("md5, others (upgrade to OpenSSL-1.0 for full list)"));
    strcpy(list, "md5, others (upgrade to OpenSSL-1.0 for full list)");
# endif
#else
    list = (char *)emalloc(sizeof("md5"));
    strcpy(list, "md5");
#endif

    return list;
}