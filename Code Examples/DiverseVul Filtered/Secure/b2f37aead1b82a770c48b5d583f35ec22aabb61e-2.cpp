static void ax_bump(struct mkiss *ax)
{
	struct sk_buff *skb;
	int count;

	spin_lock_bh(&ax->buflock);
	if (ax->rbuff[0] > 0x0f) {
		if (ax->rbuff[0] & 0x80) {
			if (check_crc_16(ax->rbuff, ax->rcount) < 0) {
				ax->dev->stats.rx_errors++;
				spin_unlock_bh(&ax->buflock);

				return;
			}
			if (ax->crcmode != CRC_MODE_SMACK && ax->crcauto) {
				printk(KERN_INFO
				       "mkiss: %s: Switching to crc-smack\n",
				       ax->dev->name);
				ax->crcmode = CRC_MODE_SMACK;
			}
			ax->rcount -= 2;
			*ax->rbuff &= ~0x80;
		} else if (ax->rbuff[0] & 0x20)  {
			if (check_crc_flex(ax->rbuff, ax->rcount) < 0) {
				ax->dev->stats.rx_errors++;
				spin_unlock_bh(&ax->buflock);
				return;
			}
			if (ax->crcmode != CRC_MODE_FLEX && ax->crcauto) {
				printk(KERN_INFO
				       "mkiss: %s: Switching to crc-flexnet\n",
				       ax->dev->name);
				ax->crcmode = CRC_MODE_FLEX;
			}
			ax->rcount -= 2;

			/*
			 * dl9sau bugfix: the trailling two bytes flexnet crc
			 * will not be passed to the kernel. thus we have to
			 * correct the kissparm signature, because it indicates
			 * a crc but there's none
			 */
			*ax->rbuff &= ~0x20;
		}
	}

	count = ax->rcount;

	if ((skb = dev_alloc_skb(count)) == NULL) {
		printk(KERN_ERR "mkiss: %s: memory squeeze, dropping packet.\n",
		       ax->dev->name);
		ax->dev->stats.rx_dropped++;
		spin_unlock_bh(&ax->buflock);
		return;
	}

	skb_put_data(skb, ax->rbuff, count);
	skb->protocol = ax25_type_trans(skb, ax->dev);
	netif_rx(skb);
	ax->dev->stats.rx_packets++;
	ax->dev->stats.rx_bytes += count;
	spin_unlock_bh(&ax->buflock);
}