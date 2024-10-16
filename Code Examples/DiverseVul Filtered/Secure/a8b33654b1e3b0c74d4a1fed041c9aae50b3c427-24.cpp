static int set_auto_rts(struct sb_uart_port *port, int status)
{
	int atr_status = 0;

#if 0
	int efr_status = 0;

	efr_status = sb1054_get_register(port, PAGE_3, SB105X_EFR);
	if(status == ENABLE)
		efr_status |= SB105X_EFR_ARTS;
	else
		efr_status &= ~SB105X_EFR_ARTS;
	sb1054_set_register(port,PAGE_3,SB105X_EFR,efr_status);
	efr_status = sb1054_get_register(port, PAGE_3, SB105X_EFR);
#endif
		
//ATR
	atr_status = sb1054_get_register(port, PAGE_3, SB105X_ATR);
	switch(status)
	{
		case RS422PTP:
			atr_status = (SB105X_ATR_TPS) | (SB105X_ATR_A80);
			break;
		case RS422MD:
			atr_status = (SB105X_ATR_TPS) | (SB105X_ATR_TCMS) | (SB105X_ATR_A80);
			break;
		case RS485NE:
			atr_status = (SB105X_ATR_RCMS) | (SB105X_ATR_TPS) | (SB105X_ATR_TCMS) | (SB105X_ATR_A80);
			break;
		case RS485ECHO:
			atr_status = (SB105X_ATR_TPS) | (SB105X_ATR_TCMS) | (SB105X_ATR_A80);
			break;
	}

	sb1054_set_register(port,PAGE_3,SB105X_ATR,atr_status);
	atr_status = sb1054_get_register(port, PAGE_3, SB105X_ATR);

	return atr_status;
}