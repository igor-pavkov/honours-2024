static void printCSS(FILE *f)
{
  // Image flip/flop CSS
  // Source:
  // http://stackoverflow.com/questions/1309055/cross-browser-way-to-flip-html-image-via-javascript-css
  // tested in Chrome, Fx (Linux) and IE9 (W7)
  static const char css[] = 
    "<style type=\"text/css\">" "\n"
    "<!--" "\n"
    ".xflip {" "\n"
    "    -moz-transform: scaleX(-1);" "\n"
    "    -webkit-transform: scaleX(-1);" "\n"
    "    -o-transform: scaleX(-1);" "\n"
    "    transform: scaleX(-1);" "\n"
    "    filter: fliph;" "\n"
    "}" "\n"
    ".yflip {" "\n"
    "    -moz-transform: scaleY(-1);" "\n"
    "    -webkit-transform: scaleY(-1);" "\n"
    "    -o-transform: scaleY(-1);" "\n"
    "    transform: scaleY(-1);" "\n"
    "    filter: flipv;" "\n"
    "}" "\n"
    ".xyflip {" "\n"
    "    -moz-transform: scaleX(-1) scaleY(-1);" "\n"
    "    -webkit-transform: scaleX(-1) scaleY(-1);" "\n"
    "    -o-transform: scaleX(-1) scaleY(-1);" "\n"
    "    transform: scaleX(-1) scaleY(-1);" "\n"
    "    filter: fliph + flipv;" "\n"
    "}" "\n"
    "-->" "\n"
    "</style>" "\n";

  fwrite( css, sizeof(css)-1, 1, f );
}