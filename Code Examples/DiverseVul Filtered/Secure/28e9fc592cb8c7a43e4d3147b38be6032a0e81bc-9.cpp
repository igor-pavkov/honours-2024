static inline u16 llc_ui_next_link_no(int sap)
{
	return llc_ui_sap_link_no_max[sap]++;
}