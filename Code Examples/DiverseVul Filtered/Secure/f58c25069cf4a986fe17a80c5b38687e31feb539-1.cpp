URI_CHAR URI_FUNC(HexToLetterEx)(unsigned int value, UriBool uppercase) {
	switch (value) {
	case  0: return _UT('0');
	case  1: return _UT('1');
	case  2: return _UT('2');
	case  3: return _UT('3');
	case  4: return _UT('4');
	case  5: return _UT('5');
	case  6: return _UT('6');
	case  7: return _UT('7');
	case  8: return _UT('8');
	case  9: return _UT('9');

	case 10: return (uppercase == URI_TRUE) ? _UT('A') : _UT('a');
	case 11: return (uppercase == URI_TRUE) ? _UT('B') : _UT('b');
	case 12: return (uppercase == URI_TRUE) ? _UT('C') : _UT('c');
	case 13: return (uppercase == URI_TRUE) ? _UT('D') : _UT('d');
	case 14: return (uppercase == URI_TRUE) ? _UT('E') : _UT('e');
	default: return (uppercase == URI_TRUE) ? _UT('F') : _UT('f');
	}
}