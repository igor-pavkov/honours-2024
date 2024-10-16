bool jslNeedSpaceBetween(unsigned char lastch, unsigned char ch) {
  return (lastch>=_LEX_R_LIST_START || ch>=_LEX_R_LIST_START) &&
         (lastch>=_LEX_R_LIST_START || isAlpha((char)lastch) || isNumeric((char)lastch)) &&
         (ch>=_LEX_R_LIST_START || isAlpha((char)ch) || isNumeric((char)ch));
}