void WriteString(Window *wp, char *buf, int len)
{
	int c;
	int font;
	Canvas *cv;

	if (!len)
		return;
	if (wp->w_log)
		WLogString(wp, buf, len);

	/* set global variables (yuck!) */
	curr = wp;
	cols = curr->w_width;
	rows = curr->w_height;

	if (curr->w_silence)
		SetTimeout(&curr->w_silenceev, curr->w_silencewait * 1000);

	if (curr->w_monitor == MON_ON) {
		curr->w_monitor = MON_FOUND;
	}

	if (cols > 0 && rows > 0) {
		do {
			c = (unsigned char)*buf++;
			if (!curr->w_mbcs)
				curr->w_rend.font = curr->w_FontL;	/* Default: GL */

			if (curr->w_encoding == UTF8) {
				c = FromUtf8(c, &curr->w_decodestate);
				if (c == -1)
					continue;
				if (c == -2) {
					c = UCS_REPL;
					/* try char again */
					buf--;
					len++;
				}
			}

 tryagain:
			switch (curr->w_state) {
			case PRIN:
				switch (c) {
				case '\033':
					curr->w_state = PRINESC;
					break;
				default:
					PrintChar(c);
				}
				break;
			case PRINESC:
				switch (c) {
				case '[':
					curr->w_state = PRINCSI;
					break;
				default:
					PrintChar('\033');
					PrintChar(c);
					curr->w_state = PRIN;
				}
				break;
			case PRINCSI:
				switch (c) {
				case '4':
					curr->w_state = PRIN4;
					break;
				default:
					PrintChar('\033');
					PrintChar('[');
					PrintChar(c);
					curr->w_state = PRIN;
				}
				break;
			case PRIN4:
				switch (c) {
				case 'i':
					curr->w_state = LIT;
					PrintFlush();
					if (curr->w_pdisplay && curr->w_pdisplay->d_printfd >= 0) {
						close(curr->w_pdisplay->d_printfd);
						curr->w_pdisplay->d_printfd = -1;
					}
					curr->w_pdisplay = 0;
					break;
				default:
					PrintChar('\033');
					PrintChar('[');
					PrintChar('4');
					PrintChar(c);
					curr->w_state = PRIN;
				}
				break;
			case ASTR:
				if (c == 0)
					break;
				if (c == '\033') {
					curr->w_state = STRESC;
					break;
				}
				/* special xterm hack: accept SetStatus sequence. Yucc! */
				/* allow ^E for title escapes */
				if (!(curr->w_StringType == OSC && c < ' ' && c != '\005'))
					if (!curr->w_c1 || c != ('\\' ^ 0xc0)) {
						StringChar(c);
						break;
					}
				c = '\\';
				/* FALLTHROUGH */
			case STRESC:
				switch (c) {
				case '\\':
					if (StringEnd() == 0 || len <= 1)
						break;
					/* check if somewhere a status is displayed */
					for (cv = curr->w_layer.l_cvlist; cv; cv = cv->c_lnext) {
						display = cv->c_display;
						if (D_status == STATUS_ON_WIN)
							break;
					}
					if (cv) {
						if (len > IOSIZE + 1)
							len = IOSIZE + 1;
						curr->w_outlen = len - 1;
						memmove(curr->w_outbuf, buf, len - 1);
						return;	/* wait till status is gone */
					}
					break;
				case '\033':
					StringChar('\033');
					break;
				default:
					curr->w_state = ASTR;
					StringChar('\033');
					StringChar(c);
					break;
				}
				break;
			case ESC:
				switch (c) {
				case '[':
					curr->w_NumArgs = 0;
					curr->w_intermediate = 0;
					memset((char *)curr->w_args, 0, MAXARGS * sizeof(int));
					curr->w_state = CSI;
					break;
				case ']':
					StringStart(OSC);
					break;
				case '_':
					StringStart(APC);
					break;
				case 'P':
					StringStart(DCS);
					break;
				case '^':
					StringStart(PM);
					break;
				case '!':
					StringStart(GM);
					break;
				case '"':
				case 'k':
					StringStart(AKA);
					break;
				default:
					if (Special(c)) {
						curr->w_state = LIT;
						break;
					}
					if (c >= ' ' && c <= '/') {
						if (curr->w_intermediate) {
							if (curr->w_intermediate == '$')
								c |= '$' << 8;
							else
								c = -1;
						}
						curr->w_intermediate = c;
					} else if (c >= '0' && c <= '~') {
						DoESC(c, curr->w_intermediate);
						curr->w_state = LIT;
					} else {
						curr->w_state = LIT;
						goto tryagain;
					}
				}
				break;
			case CSI:
				switch (c) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					if (curr->w_NumArgs >= 0 && curr->w_NumArgs < MAXARGS) {
						if (curr->w_args[curr->w_NumArgs] < 100000000)
							curr->w_args[curr->w_NumArgs] =
							    10 * curr->w_args[curr->w_NumArgs] + (c - '0');
					}
					break;
				case ';':
				case ':':
					if (curr->w_NumArgs < MAXARGS)
						curr->w_NumArgs++;
					break;
				default:
					if (Special(c))
						break;
					if (c >= '@' && c <= '~') {
						if (curr->w_NumArgs < MAXARGS)
							curr->w_NumArgs++;
						DoCSI(c, curr->w_intermediate);
						if (curr->w_state != PRIN)
							curr->w_state = LIT;
					} else if ((c >= ' ' && c <= '/') || (c >= '<' && c <= '?'))
						curr->w_intermediate = curr->w_intermediate ? -1 : c;
					else {
						curr->w_state = LIT;
						goto tryagain;
					}
				}
				break;
			case LIT:
			default:
				if (curr->w_mbcs)
					if (c <= ' ' || c == 0x7f || (c >= 0x80 && c < 0xa0 && curr->w_c1))
						curr->w_mbcs = 0;
				if (c < ' ') {
					if (c == '\033') {
						curr->w_intermediate = 0;
						curr->w_state = ESC;
						if (curr->w_autoaka < 0)
							curr->w_autoaka = 0;
					} else
						Special(c);
					break;
				}
				if (c >= 0x80 && c < 0xa0 && curr->w_c1)
					if ((curr->w_FontR & 0xf0) != 0x20 || curr->w_encoding == UTF8) {
						switch (c) {
						case 0xc0 ^ 'D':
						case 0xc0 ^ 'E':
						case 0xc0 ^ 'H':
						case 0xc0 ^ 'M':
						case 0xc0 ^ 'N':	/* SS2 */
						case 0xc0 ^ 'O':	/* SS3 */
							DoESC(c ^ 0xc0, 0);
							break;
						case 0xc0 ^ '[':
							if (curr->w_autoaka < 0)
								curr->w_autoaka = 0;
							curr->w_NumArgs = 0;
							curr->w_intermediate = 0;
							memset((char *)curr->w_args, 0, MAXARGS * sizeof(int));
							curr->w_state = CSI;
							break;
						case 0xc0 ^ 'P':
							StringStart(DCS);
							break;
						default:
							break;
						}
						break;
					}

				if (!curr->w_mbcs) {
					if (c < 0x80 || curr->w_gr == 0)
						curr->w_rend.font = curr->w_FontL;
					else if (curr->w_gr == 2 && !curr->w_ss)
						curr->w_rend.font = curr->w_FontE;
					else
						curr->w_rend.font = curr->w_FontR;
				}
				if (curr->w_encoding == UTF8) {
					if (curr->w_rend.font == '0') {
						struct mchar mc, *mcp;

						mc.image = c;
						mc.mbcs = 0;
						mc.font = '0';
						mc.fontx = 0;
						mcp = recode_mchar(&mc, 0, UTF8);
						c = mcp->image | mcp->font << 8;
					}
					curr->w_rend.font = 0;
				}
				if (curr->w_encoding == UTF8 && utf8_isdouble(c))
					curr->w_mbcs = 0xff;
				if (curr->w_encoding == UTF8 && c >= 0x0300 && utf8_iscomb(c)) {
					int ox, oy;
					struct mchar omc;

					ox = curr->w_x - 1;
					oy = curr->w_y;
					if (ox < 0) {
						ox = curr->w_width - 1;
						oy--;
					}
					if (oy < 0)
						oy = 0;
					copy_mline2mchar(&omc, &curr->w_mlines[oy], ox);
					if (omc.image == 0xff && omc.font == 0xff && omc.fontx == 0) {
						ox--;
						if (ox >= 0) {
							copy_mline2mchar(&omc, &curr->w_mlines[oy], ox);
							omc.mbcs = 0xff;
						}
					}
					if (ox >= 0) {
						utf8_handle_comb(c, &omc);
						MFixLine(curr, oy, &omc);
						copy_mchar2mline(&omc, &curr->w_mlines[oy], ox);
						LPutChar(&curr->w_layer, &omc, ox, oy);
						LGotoPos(&curr->w_layer, curr->w_x, curr->w_y);
					}
					break;
				}
				font = curr->w_rend.font;
				if (font == KANA && curr->w_encoding == SJIS && curr->w_mbcs == 0) {
					/* Lets see if it is the first byte of a kanji */
					if ((0x81 <= c && c <= 0x9f) || (0xe0 <= c && c <= 0xef)) {
						curr->w_mbcs = c;
						break;
					}
				}
				if (font == 031 && c == 0x80 && !curr->w_mbcs)
					font = curr->w_rend.font = 0;
				if (is_dw_font(font) && c == ' ')
					font = curr->w_rend.font = 0;
				if (is_dw_font(font) || curr->w_mbcs) {
					int t = c;
					if (curr->w_mbcs == 0) {
						curr->w_mbcs = c;
						break;
					}
					if (curr->w_x == cols - 1) {
						curr->w_x += curr->w_wrap ? 1 : -1;
					}
					if (curr->w_encoding != UTF8) {
						c = curr->w_mbcs;
						if (font == KANA && curr->w_encoding == SJIS) {
							/*
							 * SJIS -> EUC mapping:
							 *   First byte:
							 *     81,82...9f -> 21,23...5d
							 *     e0,e1...ef -> 5f,61...7d
							 *   Second byte:
							 *     40-7e -> 21-5f
							 *     80-9e -> 60-7e
							 *     9f-fc -> 21-7e (increment first byte!)
							 */
							if (0x40 <= t && t <= 0xfc && t != 0x7f) {
								if (c <= 0x9f)
									c = (c - 0x81) * 2 + 0x21;
								else
									c = (c - 0xc1) * 2 + 0x21;
								if (t <= 0x7e)
									t -= 0x1f;
								else if (t <= 0x9e)
									t -= 0x20;
								else
									t -= 0x7e, c++;
								curr->w_rend.font = KANJI;
							} else {
								/* Incomplete shift-jis - skip first byte */
								c = t;
								t = 0;
							}
						}
						if (t && curr->w_gr && font != 030 && font != 031) {
							t &= 0x7f;
							if (t < ' ')
								goto tryagain;
						}
						if (t == '\177')
							break;
						curr->w_mbcs = t;
					}
				}
				if (font == '<' && c >= ' ') {
					curr->w_rend.font = 0;
					c |= 0x80;
				} else if (curr->w_gr && curr->w_encoding != UTF8) {
					if (c == 0x80 && font == 0 && curr->w_encoding == GBK)
						c = 0xa4;
					else
						c &= 0x7f;
					if (c < ' ' && font != 031)
						goto tryagain;
				}
				if (c == '\177')
					break;
				curr->w_rend.image = c;
				if (curr->w_encoding == UTF8) {
					curr->w_rend.font = c >> 8;
					curr->w_rend.fontx = c >> 16;
				}
				curr->w_rend.mbcs = curr->w_mbcs;
				if (curr->w_x < cols - 1) {
					if (curr->w_insert) {
						save_mline(&curr->w_mlines[curr->w_y], cols);
						MInsChar(curr, &curr->w_rend, curr->w_x, curr->w_y);
						LInsChar(&curr->w_layer, &curr->w_rend, curr->w_x, curr->w_y,
							 &mline_old);
						curr->w_x++;
					} else {
						MPutChar(curr, &curr->w_rend, curr->w_x, curr->w_y);
						LPutChar(&curr->w_layer, &curr->w_rend, curr->w_x, curr->w_y);
						curr->w_x++;
					}
				} else if (curr->w_x == cols - 1) {
					MPutChar(curr, &curr->w_rend, curr->w_x, curr->w_y);
					LPutChar(&curr->w_layer, &curr->w_rend, curr->w_x, curr->w_y);
					if (curr->w_wrap)
						curr->w_x++;
				} else {
					MWrapChar(curr, &curr->w_rend, curr->w_y, curr->w_top, curr->w_bot,
						  curr->w_insert);
					LWrapChar(&curr->w_layer, &curr->w_rend, curr->w_y, curr->w_top, curr->w_bot,
						  curr->w_insert);
					if (curr->w_y != curr->w_bot && curr->w_y != curr->w_height - 1)
						curr->w_y++;
					curr->w_x = 1;
				}
				if (curr->w_mbcs) {
					curr->w_rend.mbcs = curr->w_mbcs = 0;
					curr->w_x++;
				}
				if (curr->w_ss) {
					curr->w_FontL = curr->w_charsets[curr->w_Charset];
					curr->w_FontR = curr->w_charsets[curr->w_CharsetR];
					curr->w_rend.font = curr->w_FontL;
					LSetRendition(&curr->w_layer, &curr->w_rend);
					curr->w_ss = 0;
				}
				break;
			}
		}
		while (--len);
	}
	if (!printcmd && curr->w_state == PRIN)
		PrintFlush();
}