static int fdb_vid_parse(struct nlattr *vlan_attr, u16 *p_vid)
{
	u16 vid = 0;

	if (vlan_attr) {
		if (nla_len(vlan_attr) != sizeof(u16)) {
			pr_info("PF_BRIDGE: RTM_NEWNEIGH with invalid vlan\n");
			return -EINVAL;
		}

		vid = nla_get_u16(vlan_attr);

		if (!vid || vid >= VLAN_VID_MASK) {
			pr_info("PF_BRIDGE: RTM_NEWNEIGH with invalid vlan id %d\n",
				vid);
			return -EINVAL;
		}
	}
	*p_vid = vid;
	return 0;
}