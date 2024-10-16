int slap_has64( int onoff )
{
	if ( onoff < 0 )
		return 0;
	else
		return onoff ? -1 : 0;
}