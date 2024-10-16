static int mboxlist_find_category(struct find_rock *rock, const char *prefix, size_t len)
{
    int r = 0;
    if (!rock->issubs && !rock->isadmin && !cyrusdb_fetch(rock->db, "$RACL", 5, NULL, NULL, NULL)) {
        /* we're using reverse ACLs */
        struct buf buf = BUF_INITIALIZER;
        strarray_t matches = STRARRAY_INITIALIZER;
        mboxlist_racl_key(rock->mb_category == MBNAME_OTHERUSER, rock->userid, NULL, &buf);
        /* this is the prefix */
        struct raclrock raclrock = { buf.len, &matches };
        /* we only need to look inside the prefix still, but we keep the length
         * in raclrock pointing to the start of the mboxname part of the key so
         * we get correct names in matches */
        if (len) buf_appendmap(&buf, prefix, len);
        r = cyrusdb_foreach(rock->db, buf.s, buf.len, NULL, racl_cb, &raclrock, NULL);
        /* XXX - later we need to sort the array when we've added groups */
        int i;
        for (i = 0; !r && i < strarray_size(&matches); i++) {
            const char *key = strarray_nth(&matches, i);
            r = cyrusdb_forone(rock->db, key, strlen(key), &find_p, &find_cb, rock, NULL);
        }
        strarray_fini(&matches);
    }
    else {
        r = cyrusdb_foreach(rock->db, prefix, len, &find_p, &find_cb, rock, NULL);
    }

    if (r == CYRUSDB_DONE) r = 0;
    return r;
}