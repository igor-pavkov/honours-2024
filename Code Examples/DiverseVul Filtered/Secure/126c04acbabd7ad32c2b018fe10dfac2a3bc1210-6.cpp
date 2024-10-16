static void ip_freef(Slirp *slirp, struct ipq *fp)
{
    register struct ipasfrag *q, *p;

    for (q = fp->frag_link.next; q != (struct ipasfrag *)&fp->frag_link;
         q = p) {
        p = q->ipf_next;
        ip_deq(q);
        m_free(dtom(slirp, q));
    }
    remque(&fp->ip_link);
    (void)m_free(dtom(slirp, fp));
}