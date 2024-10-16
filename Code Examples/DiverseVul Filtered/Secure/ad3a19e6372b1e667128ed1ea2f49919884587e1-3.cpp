help (
  void
)
{
  /* Break help string into little ones, to assure ISO C89 conformance */
  printf ("Usage: " appname " [OPTION]... [FILE]...\n"
	  "A command line interface for the " FRIBIDI_NAME " library.\n"
	  "Convert a logical string to visual.\n"
	  "\n"
	  "  -h, --help            Display this information and exit\n"
	  "  -V, --version         Display version information and exit\n"
	  "  -v, --verbose         Verbose mode, same as --basedir --ltov --vtol\n"
	  "                        --levels\n");
  printf ("  -d, --debug           Output debug information\n"
	  "  -t, --test            Test " FRIBIDI_NAME
	  ", same as --clean --nobreak\n"
	  "                        --showinput --reordernsm --width %d\n",
	  default_text_width);
  printf ("  -c, --charset CS      Specify character set, default is %s\n"
	  "      --charsetdesc CS  Show descriptions for character set CS and exit\n"
	  "      --caprtl          Old style: set character set to CapRTL\n",
	  char_set);
  printf ("      --showinput       Output the input string too\n"
	  "      --nopad           Do not right justify RTL lines\n"
	  "      --nobreak         Do not break long lines\n"
	  "  -w, --width W         Screen width for padding, default is %d, but if\n"
	  "                        environment variable COLUMNS is defined, its value\n"
	  "                        will be used, --width overrides both of them.\n",
	  default_text_width);
  printf
    ("  -B, --bol BOL         Output string BOL before the visual string\n"
     "  -E, --eol EOL         Output string EOL after the visual string\n"
     "      --rtl             Force base direction to RTL\n"
     "      --ltr             Force base direction to LTR\n"
     "      --wrtl            Set base direction to RTL if no strong character found\n");
  printf
    ("      --wltr            Set base direction to LTR if no strong character found\n"
     "                        (default)\n"
     "      --nomirror        Turn mirroring off, to do it later\n"
     "      --reordernsm      Reorder NSM sequences to follow their base character\n"
     "      --clean           Remove explicit format codes in visual string\n"
     "                        output, currently does not affect other outputs\n"
     "      --basedir         Output Base Direction\n");
  printf ("      --ltov            Output Logical to Visual position map\n"
	  "      --vtol            Output Visual to Logical position map\n"
	  "      --levels          Output Embedding Levels\n"
	  "      --novisual        Do not output the visual string, to be used with\n"
	  "                        --basedir, --ltov, --vtol, --levels\n");
  printf ("  All string indexes are zero based\n" "\n" "Output:\n"
	  "  For each line of input, output something like this:\n"
	  "    [input-str` => '][BOL][[padding space]visual-str][EOL]\n"
	  "    [\\n base-dir][\\n ltov-map][\\n vtol-map][\\n levels]\n");

  {
    int i;
    printf ("\n" "Available character sets:\n");
    for (i = 1; i <= FRIBIDI_CHAR_SETS_NUM; i++)
      printf ("  * %-10s: %-25s%1s\n",
	      fribidi_char_set_name (i), fribidi_char_set_title (i),
	      (fribidi_char_set_desc (i) ? "X" : ""));
    printf
      ("  X: Character set has descriptions, use --charsetdesc to see\n");
  }

  printf ("\nReport bugs online at\n<" FRIBIDI_BUGREPORT ">.\n");
  exit (0);
}