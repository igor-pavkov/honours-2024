static int mboxlist_create_acl(const char *mboxname, char **out)
{
    mbentry_t *mbentry = NULL;
    int r;
    int mask;

    char *defaultacl;
    char *identifier;
    char *rights;
    char *p;

    r = mboxlist_findparent(mboxname, &mbentry);
    if (!r) {
        *out = xstrdup(mbentry->acl);
        mboxlist_entry_free(&mbentry);
        return 0;
    }

    *out = xstrdup("");
    char *owner = mboxname_to_userid(mboxname);
    if (owner) {
        /* owner gets full permission on own mailbox by default */
        cyrus_acl_set(out, owner, ACL_MODE_SET, ACL_ALL,
                      (cyrus_acl_canonproc_t *)0, (void *)0);
        free(owner);
        return 0;
    }

    defaultacl = identifier = xstrdup(config_getstring(IMAPOPT_DEFAULTACL));
    for (;;) {
        while (*identifier && Uisspace(*identifier)) identifier++;
        rights = identifier;
        while (*rights && !Uisspace(*rights)) rights++;
        if (!*rights) break;
        *rights++ = '\0';
        while (*rights && Uisspace(*rights)) rights++;
        if (!*rights) break;
        p = rights;
        while (*p && !Uisspace(*p)) p++;
        if (*p) *p++ = '\0';
        cyrus_acl_strtomask(rights, &mask);
        /* XXX and if strtomask fails? */
        cyrus_acl_set(out, identifier, ACL_MODE_SET, mask,
                      (cyrus_acl_canonproc_t *)0, (void *)0);
        identifier = p;
    }
    free(defaultacl);

    return 0;
}