void gdDumpRect(const char *msg, gdRectPtr r)
{
	printf("%s (%i, %i) (%i, %i)\n", msg, r->x, r->y, r->width, r->height);
}