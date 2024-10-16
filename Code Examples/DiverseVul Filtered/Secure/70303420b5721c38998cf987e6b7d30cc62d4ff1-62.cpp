static void parse_error(struct filter_parse_error *pe, int err, int pos)
{
	pe->lasterr = err;
	pe->lasterr_pos = pos;
}