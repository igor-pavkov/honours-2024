static char* ReadPassword(rfbClient* client) {
#ifdef __MINGW32__
	/* FIXME */
	rfbClientErr("ReadPassword on MinGW32 NOT IMPLEMENTED\n");
	return NoPassword(client);
#else
	int i;
	char* p=malloc(9);
	struct termios save,noecho;
	p[0]=0;
	if(tcgetattr(fileno(stdin),&save)!=0) return p;
	noecho=save; noecho.c_lflag &= ~ECHO;
	if(tcsetattr(fileno(stdin),TCSAFLUSH,&noecho)!=0) return p;
	fprintf(stderr,"Password: ");
	i=0;
	while(1) {
		int c=fgetc(stdin);
		if(c=='\n')
			break;
		if(i<8) {
			p[i]=c;
			i++;
			p[i]=0;
		}
	}
	tcsetattr(fileno(stdin),TCSAFLUSH,&save);
	return p;
#endif
}