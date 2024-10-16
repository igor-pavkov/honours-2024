static int bcm_char_release(struct inode *inode, struct file *filp)
{
	struct bcm_tarang_data *pTarang, *tmp, *ptmp;
	struct bcm_mini_adapter *Adapter = NULL;
	struct sk_buff *pkt, *npkt;

	pTarang = (struct bcm_tarang_data *)filp->private_data;

	if (pTarang == NULL) {
		BCM_DEBUG_PRINT(Adapter, DBG_TYPE_PRINTK, 0, 0,
				"ptarang is null\n");
		return 0;
	}

	Adapter = pTarang->Adapter;

	down(&Adapter->RxAppControlQueuelock);

	tmp = Adapter->pTarangs;
	for (ptmp = NULL; tmp; ptmp = tmp, tmp = tmp->next) {
		if (tmp == pTarang)
			break;
	}

	if (tmp) {
		if (!ptmp)
			Adapter->pTarangs = tmp->next;
		else
			ptmp->next = tmp->next;
	} else {
		up(&Adapter->RxAppControlQueuelock);
		return 0;
	}

	pkt = pTarang->RxAppControlHead;
	while (pkt) {
		npkt = pkt->next;
		kfree_skb(pkt);
		pkt = npkt;
	}

	up(&Adapter->RxAppControlQueuelock);

	/* Stop Queuing the control response Packets */
	atomic_dec(&Adapter->ApplicationRunning);

	kfree(pTarang);

	/* remove this filp from the asynchronously notified filp's */
	filp->private_data = NULL;
	return 0;
}