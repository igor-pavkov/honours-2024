void qdisc_warn_nonwc(char *txt, struct Qdisc *qdisc)
{
	if (!(qdisc->flags & TCQ_F_WARN_NONWC)) {
		printk(KERN_WARNING
		       "%s: %s qdisc %X: is non-work-conserving?\n",
		       txt, qdisc->ops->id, qdisc->handle >> 16);
		qdisc->flags |= TCQ_F_WARN_NONWC;
	}
}