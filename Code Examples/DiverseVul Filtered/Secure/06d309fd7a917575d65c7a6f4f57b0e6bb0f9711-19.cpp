static void php_iconv_stream_filter_dtor(php_iconv_stream_filter *self)
{
	iconv_close(self->cd);
	pefree(self->to_charset, self->persistent);
	pefree(self->from_charset, self->persistent);
}