static u64 muldiv64(u64 a, u32 b, u32 c)
{
	union {
		u64 ll;
		struct {
			u32 low, high;
		} l;
	} u, res;
	u64 rl, rh;

	u.ll = a;
	rl = (u64)u.l.low * (u64)b;
	rh = (u64)u.l.high * (u64)b;
	rh += (rl >> 32);
	res.l.high = div64_u64(rh, c);
	res.l.low = div64_u64(((mod_64(rh, c) << 32) + (rl & 0xffffffff)), c);
	return res.ll;
}