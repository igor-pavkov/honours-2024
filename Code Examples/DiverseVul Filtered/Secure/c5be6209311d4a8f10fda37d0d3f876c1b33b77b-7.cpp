static kadm5_ret_t add_to_history(krb5_context context,
                                  krb5_kvno hist_kvno,
                                  osa_princ_ent_t adb,
                                  kadm5_policy_ent_t pol,
                                  osa_pw_hist_ent *pw)
{
    osa_pw_hist_ent *histp;
    uint32_t nhist;
    unsigned int i, knext, nkeys;

    nhist = pol->pw_history_num;
    /* A history of 1 means just check the current password */
    if (nhist <= 1)
        return 0;

    if (adb->admin_history_kvno != hist_kvno) {
        /* The history key has changed since the last password change, so we
         * have to reset the password history. */
        free(adb->old_keys);
        adb->old_keys = NULL;
        adb->old_key_len = 0;
        adb->old_key_next = 0;
        adb->admin_history_kvno = hist_kvno;
    }

    nkeys = adb->old_key_len;
    knext = adb->old_key_next;
    /* resize the adb->old_keys array if necessary */
    if (nkeys + 1 < nhist) {
        if (adb->old_keys == NULL) {
            adb->old_keys = (osa_pw_hist_ent *)
                malloc((nkeys + 1) * sizeof (osa_pw_hist_ent));
        } else {
            adb->old_keys = (osa_pw_hist_ent *)
                realloc(adb->old_keys,
                        (nkeys + 1) * sizeof (osa_pw_hist_ent));
        }
        if (adb->old_keys == NULL)
            return(ENOMEM);

        memset(&adb->old_keys[nkeys], 0, sizeof(osa_pw_hist_ent));
        nkeys = ++adb->old_key_len;
        /*
         * To avoid losing old keys, shift forward each entry after
         * knext.
         */
        for (i = nkeys - 1; i > knext; i--) {
            adb->old_keys[i] = adb->old_keys[i - 1];
        }
        memset(&adb->old_keys[knext], 0, sizeof(osa_pw_hist_ent));
    } else if (nkeys + 1 > nhist) {
        /*
         * The policy must have changed!  Shrink the array.
         * Can't simply realloc() down, since it might be wrapped.
         * To understand the arithmetic below, note that we are
         * copying into new positions 0 .. N-1 from old positions
         * old_key_next-N .. old_key_next-1, modulo old_key_len,
         * where N = pw_history_num - 1 is the length of the
         * shortened list.        Matt Crawford, FNAL
         */
        /*
         * M = adb->old_key_len, N = pol->pw_history_num - 1
         *
         * tmp[0] .. tmp[N-1] = old[(knext-N)%M] .. old[(knext-1)%M]
         */
        int j;
        osa_pw_hist_t tmp;

        tmp = (osa_pw_hist_ent *)
            malloc((nhist - 1) * sizeof (osa_pw_hist_ent));
        if (tmp == NULL)
            return ENOMEM;
        for (i = 0; i < nhist - 1; i++) {
            /*
             * Add nkeys once before taking remainder to avoid
             * negative values.
             */
            j = (i + nkeys + knext - (nhist - 1)) % nkeys;
            tmp[i] = adb->old_keys[j];
        }
        /* Now free the ones we don't keep (the oldest ones) */
        for (i = 0; i < nkeys - (nhist - 1); i++) {
            j = (i + nkeys + knext) % nkeys;
            histp = &adb->old_keys[j];
            for (j = 0; j < histp->n_key_data; j++) {
                krb5_free_key_data_contents(context, &histp->key_data[j]);
            }
            free(histp->key_data);
        }
        free(adb->old_keys);
        adb->old_keys = tmp;
        nkeys = adb->old_key_len = nhist - 1;
        knext = adb->old_key_next = 0;
    }

    /*
     * If nhist decreased since the last password change, and nkeys+1
     * is less than the previous nhist, it is possible for knext to
     * index into unallocated space.  This condition would not be
     * caught by the resizing code above.
     */
    if (knext + 1 > nkeys)
        knext = adb->old_key_next = 0;
    /* free the old pw history entry if it contains data */
    histp = &adb->old_keys[knext];
    for (i = 0; i < (unsigned int) histp->n_key_data; i++)
        krb5_free_key_data_contents(context, &histp->key_data[i]);
    free(histp->key_data);

    /* store the new entry */
    adb->old_keys[knext] = *pw;

    /* update the next pointer */
    if (++adb->old_key_next == nhist - 1)
        adb->old_key_next = 0;

    return(0);
}