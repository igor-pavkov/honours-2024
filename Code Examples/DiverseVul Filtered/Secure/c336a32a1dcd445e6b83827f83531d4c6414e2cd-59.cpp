static void Scroll(char *cp, int cnt1, int cnt2, char *tmp)
{
	if (!cnt1 || !cnt2)
		return;
	if (cnt1 <= cnt2) {
		memmove(tmp, cp, cnt1);
		memmove(cp, cp + cnt1, cnt2);
		memmove(cp + cnt2, tmp, cnt1);
	} else {
		memmove(tmp, cp + cnt1, cnt2);
		memmove(cp + cnt2, cp, cnt1);
		memmove(cp, tmp, cnt2);
	}
}