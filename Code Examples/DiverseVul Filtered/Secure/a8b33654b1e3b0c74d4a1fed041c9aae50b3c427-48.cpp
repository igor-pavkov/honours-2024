static int get_device_type(int arg)
{
	int ret;
        ret = inb(mp_devs[arg].option_reg_addr+MP_OPTR_DIR0);
        ret = (ret & 0xf0) >> 4;
        switch (ret)
        {
               case DIR_UART_16C550:
                    return PORT_16C55X;
               case DIR_UART_16C1050:
                    return PORT_16C105X;
               case DIR_UART_16C1050A:
               /*
               if (mtpt->port.line < 2)
               {
                    return PORT_16C105XA;
               }
               else
               {
                   if (mtpt->device->device_id & 0x50)
                   {
                       return PORT_16C55X;
                   }
                   else
                   {
                       return PORT_16C105X;
                   }
               }*/
               return PORT_16C105XA;
               default:
                    return PORT_UNKNOWN;
        }

}