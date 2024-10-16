static void ip_deq(register struct ipasfrag *p)
{
    ((struct ipasfrag *)(p->ipf_prev))->ipf_next = p->ipf_next;
    ((struct ipasfrag *)(p->ipf_next))->ipf_prev = p->ipf_prev;
}