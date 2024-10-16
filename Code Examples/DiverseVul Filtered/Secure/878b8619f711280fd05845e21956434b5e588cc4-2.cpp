static inline void highlight(const int s, const int e)
{
	invert_screen(sel_cons, s, e-s+2, 1);
}