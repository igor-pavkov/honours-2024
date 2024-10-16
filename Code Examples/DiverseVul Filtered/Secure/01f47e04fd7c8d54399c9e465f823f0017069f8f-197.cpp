void Lua::purifyHTTPParameter(char *param) {
  char *ampercent;

  if((ampercent = strchr(param, '%')) != NULL) {
    /* We allow only a few chars, removing all the others */

    if((ampercent[1] != 0) && (ampercent[2] != 0)) {
      char c;
      char b = ampercent[3];

      ampercent[3] = '\0';
      c = (char)strtol(&ampercent[1], NULL, 16);
      ampercent[3] = b;

      switch(c) {
      case '/':
      case ':':
      case '(':
      case ')':
      case '{':
      case '}':
      case '[':
      case ']':
      case '?':
      case '!':
      case '$':
      case ',':
      case '^':
      case '*':
      case '_':
      case '&':
      case ' ':
      case '=':
      case '<':
      case '>':
      case '@':
      case '#':
	break;

      default:
	if(!Utils::isPrintableChar(c)) {
	  ntop->getTrace()->traceEvent(TRACE_WARNING, "Discarded char '0x%02x' in URI [%s]", c, param);
	  ampercent[0] = '\0';
	  return;
	}
      }

      purifyHTTPParameter(&ampercent[3]);
    } else
      ampercent[0] = '\0';
  }
}