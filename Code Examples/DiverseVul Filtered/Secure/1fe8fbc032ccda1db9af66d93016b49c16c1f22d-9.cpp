static int guess_day_name(const char * message, size_t length, size_t indx)
{
  int state;

  state = DAY_NAME_START;

  while (1) {

    if (indx >= length)
      return -1;

    switch(state) {
    case DAY_NAME_START:
      switch((char) toupper((unsigned char) message[indx])) {
      case 'M': /* Mon */
	return 1;
	break;
      case 'T': /* Tue Thu */
	state = DAY_NAME_T;
	break;
      case 'W': /* Wed */
	return 3;
      case 'F':
	return 5;
      case 'S': /* Sat Sun */
	state = DAY_NAME_S;
	break;
      default:
	return -1;
      }
      break;
    case DAY_NAME_T:
      switch((char) toupper((unsigned char) message[indx])) {
      case 'U':
	return 2;
      case 'H':
	return 4;
      default:
	return -1;
      }
      break;
    case DAY_NAME_S:
      switch((char) toupper((unsigned char) message[indx])) {
      case 'A':
	return 6;
      case 'U':
	return 7;
      default:
	return -1;
      }
      break;
    }

    indx ++;
  }
}