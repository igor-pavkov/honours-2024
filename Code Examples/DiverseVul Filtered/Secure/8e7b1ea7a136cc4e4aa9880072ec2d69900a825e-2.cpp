void init_openvpn_dissector(struct ndpi_detection_module_struct *ndpi_struct,
			    u_int32_t *id, NDPI_PROTOCOL_BITMASK *detection_bitmask) {
  ndpi_set_bitmask_protocol_detection("OpenVPN", ndpi_struct, detection_bitmask, *id,
				      NDPI_PROTOCOL_OPENVPN,
				      ndpi_search_openvpn,
				      NDPI_SELECTION_BITMASK_PROTOCOL_TCP_OR_UDP_WITH_PAYLOAD,
				      SAVE_DETECTION_BITMASK_AS_UNKNOWN,
				      ADD_TO_DETECTION_BITMASK);

  *id += 1;
}