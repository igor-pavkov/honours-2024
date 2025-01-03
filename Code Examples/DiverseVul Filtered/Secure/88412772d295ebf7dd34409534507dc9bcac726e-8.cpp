static int date_from_ISO8601 (const char *text, time_t * value) {
   struct tm tm;
   int n;
   int i;
   char buf[30];
	

	if (strchr (text, '-')) {
		char *p = (char *) text, *p2 = buf;
		while (p && *p) {
			if (*p != '-') {
				*p2 = *p;
				p2++;
				if (p2-buf >= sizeof(buf)) {
					return -1;
				}
			}
			p++;
		}
			text = buf;
	}


   tm.tm_isdst = -1;

#define XMLRPC_IS_NUMBER(x) if (x < '0' || x > '9') return -1;

   n = 1000;
   tm.tm_year = 0;
   for(i = 0; i < 4; i++) {
      XMLRPC_IS_NUMBER(text[i])
      tm.tm_year += (text[i]-'0')*n;
      n /= 10;
   }
   n = 10;
   tm.tm_mon = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+4])
      tm.tm_mon += (text[i+4]-'0')*n;
      n /= 10;
   }
   tm.tm_mon --;
   if(tm.tm_mon < 0 || tm.tm_mon > 11) {
       return -1;
   }

   n = 10;
   tm.tm_mday = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+6])
      tm.tm_mday += (text[i+6]-'0')*n;
      n /= 10;
   }

   n = 10;
   tm.tm_hour = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+9])
      tm.tm_hour += (text[i+9]-'0')*n;
      n /= 10;
   }

   n = 10;
   tm.tm_min = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+12])
      tm.tm_min += (text[i+12]-'0')*n;
      n /= 10;
   }

   n = 10;
   tm.tm_sec = 0;
   for(i = 0; i < 2; i++) {
      XMLRPC_IS_NUMBER(text[i+15])
      tm.tm_sec += (text[i+15]-'0')*n;
      n /= 10;
   }

   tm.tm_year -= 1900;

   *value = mkgmtime(&tm);

   return 0;

}