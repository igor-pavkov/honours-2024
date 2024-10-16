static int bcm_char_open(struct inode *inode, struct file *filp)
{
	struct bcm_mini_adapter *Adapter = NULL;
	struct bcm_tarang_data *pTarang = NULL;

	Adapter = GET_BCM_ADAPTER(gblpnetdev);
	pTarang = kzalloc(sizeof(struct bcm_tarang_data), GFP_KERNEL);
	if (!pTarang)
		return -ENOMEM;

	pTarang->Adapter = Adapter;
	pTarang->RxCntrlMsgBitMask = 0xFFFFFFFF & ~(1 << 0xB);

	down(&Adapter->RxAppControlQueuelock);
	pTarang->next = Adapter->pTarangs;
	Adapter->pTarangs = pTarang;
	up(&Adapter->RxAppControlQueuelock);

	/* Store the Adapter structure */
	filp->private_data = pTarang;

	/* Start Queuing the control response Packets */
	atomic_inc(&Adapter->ApplicationRunning);

	nonseekable_open(inode, filp);
	return 0;
}