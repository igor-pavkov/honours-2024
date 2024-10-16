_PUBLIC_ NTSTATUS nbt_rcode_to_ntstatus(uint8_t rcode)
{
	size_t i;
	struct {
		enum nbt_rcode rcode;
		NTSTATUS status;
	} map[] = {
		{ NBT_RCODE_FMT, NT_STATUS_INVALID_PARAMETER },
		{ NBT_RCODE_SVR, NT_STATUS_SERVER_DISABLED },
		{ NBT_RCODE_NAM, NT_STATUS_OBJECT_NAME_NOT_FOUND },
		{ NBT_RCODE_IMP, NT_STATUS_NOT_SUPPORTED },
		{ NBT_RCODE_RFS, NT_STATUS_ACCESS_DENIED },
		{ NBT_RCODE_ACT, NT_STATUS_ADDRESS_ALREADY_EXISTS },
		{ NBT_RCODE_CFT, NT_STATUS_CONFLICTING_ADDRESSES }
	};
	for (i=0;i<ARRAY_SIZE(map);i++) {
		if (map[i].rcode == rcode) {
			return map[i].status;
		}
	}
	return NT_STATUS_UNSUCCESSFUL;
}