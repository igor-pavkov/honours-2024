static int check_bytes(const ut8 *buf, ut64 length) {
	if (!buf || length < 8) {
		return false;
	}
	// Non-extended opcode dex file
	if (!memcmp (buf, "dex\n035\0", 8)) {
		return true;
	}
	// Extended (jumnbo) opcode dex file, ICS+ only (sdk level 14+)
	if (!memcmp (buf, "dex\n036\0", 8)) {
		return true;
	}
	// M3 (Nov-Dec 07)
	if (!memcmp (buf, "dex\n009\0", 8)) {
		return true;
	}
	// M5 (Feb-Mar 08)
	if (!memcmp (buf, "dex\n009\0", 8)) {
		return true;
	}
	// Default fall through, should still be a dex file
	if (!memcmp (buf, "dex\n", 4)) {
		return true;
	}
	return false;
}