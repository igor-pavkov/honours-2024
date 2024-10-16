static void arp_print(struct arp_payload *payload)
{
#define HBUFFERLEN 30
	char hbuffer[HBUFFERLEN];
	int j,k;

	for (k=0, j=0; k < HBUFFERLEN-3 && j < ETH_ALEN; j++) {
		hbuffer[k++] = hex_asc_hi(payload->src_hw[j]);
		hbuffer[k++] = hex_asc_lo(payload->src_hw[j]);
		hbuffer[k++]=':';
	}
	hbuffer[--k]='\0';

	pr_debug("src %pI4@%s, dst %pI4\n",
		 &payload->src_ip, hbuffer, &payload->dst_ip);
}