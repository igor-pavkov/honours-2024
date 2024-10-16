void WMsg(Window *p, int err, char *str)
{
	Layer *flayer;
	Layer *oldflayer = flayer;
	flayer = &p->w_layer;
	LMsg(err, "%s", str);
	flayer = oldflayer;
}