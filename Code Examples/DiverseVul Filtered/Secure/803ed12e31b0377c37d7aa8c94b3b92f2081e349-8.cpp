    OVS_REQUIRES(ipf->ipf_lock)
{
    enum ipf_list_state curr_state = ipf_list->state;
    enum ipf_list_state next_state;
    switch (curr_state) {
    case IPF_LIST_STATE_UNUSED:
    case IPF_LIST_STATE_OTHER_SEEN:
        if (ff) {
            next_state = IPF_LIST_STATE_FIRST_SEEN;
        } else if (lf) {
            next_state = IPF_LIST_STATE_LAST_SEEN;
        } else {
            next_state = IPF_LIST_STATE_OTHER_SEEN;
        }
        break;
    case IPF_LIST_STATE_FIRST_SEEN:
        if (lf) {
            next_state = IPF_LIST_STATE_FIRST_LAST_SEEN;
        } else {
            next_state = IPF_LIST_STATE_FIRST_SEEN;
        }
        break;
    case IPF_LIST_STATE_LAST_SEEN:
        if (ff) {
            next_state = IPF_LIST_STATE_FIRST_LAST_SEEN;
        } else {
            next_state = IPF_LIST_STATE_LAST_SEEN;
        }
        break;
    case IPF_LIST_STATE_FIRST_LAST_SEEN:
        next_state = IPF_LIST_STATE_FIRST_LAST_SEEN;
        break;
    case IPF_LIST_STATE_COMPLETED:
    case IPF_LIST_STATE_REASS_FAIL:
    case IPF_LIST_STATE_NUM:
    default:
        OVS_NOT_REACHED();
    }

    if (next_state == IPF_LIST_STATE_FIRST_LAST_SEEN) {
        ipf_sort(ipf_list->frag_list, ipf_list->last_inuse_idx);
        if (ipf_list_complete(ipf_list)) {
            struct dp_packet *reass_pkt = v6
                ? ipf_reassemble_v6_frags(ipf_list)
                : ipf_reassemble_v4_frags(ipf_list);
            if (reass_pkt) {
                struct reassembled_pkt *rp = xzalloc(sizeof *rp);
                rp->pkt = reass_pkt;
                rp->list = ipf_list;
                ipf_reassembled_list_add(&ipf->reassembled_pkt_list, rp);
                ipf_expiry_list_remove(ipf_list);
                next_state = IPF_LIST_STATE_COMPLETED;
            } else {
                next_state = IPF_LIST_STATE_REASS_FAIL;
            }
        }
    }
    ipf_list->state = next_state;
}