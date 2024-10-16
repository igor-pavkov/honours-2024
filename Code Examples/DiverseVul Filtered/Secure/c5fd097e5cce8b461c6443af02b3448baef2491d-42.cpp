HTTP_Init(void)
{
	struct vsb *vsb;

#define HTTPH(a, b, c) http_init_hdr(b, c);
#include "tbl/http_headers.h"

	vsb = VSB_new_auto();
	AN(vsb);
	VSB_printf(vsb, "1.1 %s (Varnish/" PACKAGE_BRANCH ")",
	    heritage.identity);
	AZ(VSB_finish(vsb));
	REPLACE(via_hdr, VSB_data(vsb));
	VSB_destroy(&vsb);
}