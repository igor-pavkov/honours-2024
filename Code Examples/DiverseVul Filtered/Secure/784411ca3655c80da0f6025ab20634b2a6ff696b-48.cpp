void resolvePorts(edge_t * e)
{
    if (ED_tail_port(e).dyna)
	ED_tail_port(e) =
	    resolvePort(agtail(e), aghead(e), &ED_tail_port(e));
    if (ED_head_port(e).dyna)
	ED_head_port(e) =
	    resolvePort(aghead(e), agtail(e), &ED_head_port(e));
}