    OVS_REQUIRES(ipf->ipf_lock)
{
    struct ipf_list_key key;
    /* Initialize 4 variables for some versions of GCC. */
    uint16_t start_data_byte = 0;
    uint16_t end_data_byte = 0;
    bool ff = false;
    bool lf = false;
    bool v6 = dl_type == htons(ETH_TYPE_IPV6);

    if (v6 && ipf_get_v6_enabled(ipf)) {
        ipf_v6_key_extract(pkt, dl_type, zone, &key, &start_data_byte,
                           &end_data_byte, &ff, &lf);
    } else if (!v6 && ipf_get_v4_enabled(ipf)) {
        ipf_v4_key_extract(pkt, dl_type, zone, &key, &start_data_byte,
                           &end_data_byte, &ff, &lf);
    } else {
        return false;
    }

    unsigned int nfrag_max;
    atomic_read_relaxed(&ipf->nfrag_max, &nfrag_max);
    if (atomic_count_get(&ipf->nfrag) >= nfrag_max) {
        return false;
    }

    uint32_t hash = ipf_list_key_hash(&key, hash_basis);
    struct ipf_list *ipf_list = ipf_list_key_lookup(ipf, &key, hash);
    enum {
        IPF_FRAG_LIST_MIN_INCREMENT = 4,
        IPF_IPV6_MAX_FRAG_LIST_SIZE = 65535,
    };

    int max_frag_list_size;
    if (v6) {
        /* Because the calculation with extension headers is variable,
         * we don't calculate a hard maximum fragment list size upfront.  The
         * fragment list size is practically limited by the code, however. */
        max_frag_list_size = IPF_IPV6_MAX_FRAG_LIST_SIZE;
    } else {
        max_frag_list_size = ipf->max_v4_frag_list_size;
    }

    if (!ipf_list) {
        ipf_list = xmalloc(sizeof *ipf_list);
        ipf_list_init(ipf_list, &key,
                      MIN(max_frag_list_size, IPF_FRAG_LIST_MIN_INCREMENT));
        hmap_insert(&ipf->frag_lists, &ipf_list->node, hash);
        ipf_expiry_list_add(&ipf->frag_exp_list, ipf_list, now);
    } else if (ipf_list->state == IPF_LIST_STATE_REASS_FAIL ||
               ipf_list->state == IPF_LIST_STATE_COMPLETED) {
        /* Bail out as early as possible. */
        return false;
    } else if (ipf_list->last_inuse_idx + 1 >= ipf_list->size) {
        int increment = MIN(IPF_FRAG_LIST_MIN_INCREMENT,
                            max_frag_list_size - ipf_list->size);
        /* Enforce limit. */
        if (increment > 0) {
            ipf_list->frag_list =
                xrealloc(ipf_list->frag_list, (ipf_list->size + increment) *
                  sizeof *ipf_list->frag_list);
            ipf_list->size += increment;
        } else {
            return false;
        }
    }

    return ipf_process_frag(ipf, ipf_list, pkt, start_data_byte,
                            end_data_byte, ff, lf, v6);
}