static bool tc_qdisc_dump_ignore(struct Qdisc *q)
{
	return (q->flags & TCQ_F_BUILTIN) ? true : false;
}