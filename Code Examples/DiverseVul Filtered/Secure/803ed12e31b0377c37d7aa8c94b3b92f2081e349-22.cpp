    OVS_REQUIRES(ipf->ipf_lock)
{
    bool duped_frag = ipf_is_frag_duped(ipf_list->frag_list,
        ipf_list->last_inuse_idx, start_data_byte, end_data_byte);
    int last_inuse_idx = ipf_list->last_inuse_idx;

    if (!duped_frag) {
        if (last_inuse_idx < ipf_list->size - 1) {
            /* In the case of dpdk, it would be unfortunate if we had
             * to create a clone fragment outside the dpdk mp due to the
             * mempool size being too limited. We will otherwise need to
             * recommend not setting the mempool number of buffers too low
             * and also clamp the number of fragments. */
            struct ipf_frag *frag = &ipf_list->frag_list[last_inuse_idx + 1];
            frag->pkt = dp_packet_clone(pkt);
            frag->start_data_byte = start_data_byte;
            frag->end_data_byte = end_data_byte;
            ipf_list->last_inuse_idx++;
            atomic_count_inc(&ipf->nfrag);
            ipf_count(ipf, v6, IPF_NFRAGS_ACCEPTED);
            ipf_list_state_transition(ipf, ipf_list, ff, lf, v6);
        } else {
            OVS_NOT_REACHED();
        }
    } else {
        ipf_count(ipf, v6, IPF_NFRAGS_OVERLAP);
        pkt->md.ct_state = CS_INVALID;
        return false;
    }
    return true;
}