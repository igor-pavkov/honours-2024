static inline void ConvertToNWfromDWORD(__u16 v0, __u16 v1, __u8 ret[6])
{
	__le16 *dest = (__le16 *) ret;
	dest[1] = cpu_to_le16(v0);
	dest[2] = cpu_to_le16(v1);
	dest[0] = cpu_to_le16(v0 + 1);
	return;
}