    OVS_REQUIRES(ipf->ipf_lock)
{
    struct ipf_list *ipf_list;
    HMAP_FOR_EACH_WITH_HASH (ipf_list, node, hash, &ipf->frag_lists) {
        if (ipf_list_key_eq(&ipf_list->key, key)) {
            return ipf_list;
        }
    }
    return NULL;
}