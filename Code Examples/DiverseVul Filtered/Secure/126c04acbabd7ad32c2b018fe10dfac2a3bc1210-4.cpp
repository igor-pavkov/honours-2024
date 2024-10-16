void ip_slowtimo(Slirp *slirp)
{
    struct qlink *l;

    DEBUG_CALL("ip_slowtimo");

    l = slirp->ipq.ip_link.next;

    if (l == NULL)
        return;

    while (l != &slirp->ipq.ip_link) {
        struct ipq *fp = container_of(l, struct ipq, ip_link);
        l = l->next;
        if (--fp->ipq_ttl == 0) {
            ip_freef(slirp, fp);
        }
    }
}