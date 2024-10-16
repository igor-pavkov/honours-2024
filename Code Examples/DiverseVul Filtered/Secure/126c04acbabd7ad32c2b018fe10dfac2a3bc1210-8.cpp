static void ip_enq(register struct ipasfrag *p, register struct ipasfrag *prev)
{
    DEBUG_CALL("ip_enq");
    DEBUG_ARG("prev = %p", prev);
    p->ipf_prev = prev;
    p->ipf_next = prev->ipf_next;
    ((struct ipasfrag *)(prev->ipf_next))->ipf_prev = p;
    prev->ipf_next = p;
}