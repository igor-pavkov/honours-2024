static int __init multi_init(void)
{
	int ret, i;
	struct pci_dev  *dev = NULL;

	if(fcr_count==0)
	{
		for(i=0;i<256;i++)
		{
			fcr_arr[i] = 0x01;
			
		}
	}
	if(deep_count==0)
	{
		for(i=0;i<256;i++)
		{
			deep[i] = 1;
			
		}
	}
	if(rtr_count==0)
        {
                for(i=0;i<256;i++)
                {
                        rtr[i] = 0x10;
                }
        }
	if(ttr_count==0)
        {
                for(i=0;i<256;i++)
                {
                        ttr[i] = 0x38;
                }
        }


printk("MULTI INIT\n");
	for( i=0; i< mp_nrpcibrds; i++)
	{

		while( (dev = pci_get_device(mp_pciboards[i].vendor_id, mp_pciboards[i].device_id, dev) ) )

		{
printk("FOUND~~~\n");
//	Cent OS bug fix
//			if (mp_pciboards[i].device_id & 0x0800)
			{
				int status;
	        		pci_disable_device(dev);
	        		status = pci_enable_device(dev);
            
	   		     	if (status != 0)
        			{ 
               				printk("Multiport Board Enable Fail !\n\n");
               				status = -ENXIO;
                			return status;
           			}
			}

			init_mp_dev(dev, mp_pciboards[i]);	
		}
	}

	for (i = 0; i < NR_IRQS; i++)
		spin_lock_init(&irq_lists[i].lock);

	ret = mp_register_driver(&multi_reg);

	if (ret >= 0)
		multi_register_ports(&multi_reg);

	return ret;
}