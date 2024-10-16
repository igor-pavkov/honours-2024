static void smtcfb_setmode(struct smtcfb_info *sfb)
{
	switch (sfb->fb->var.bits_per_pixel) {
	case 32:
		sfb->fb->fix.visual       = FB_VISUAL_TRUECOLOR;
		sfb->fb->fix.line_length  = sfb->fb->var.xres * 4;
		sfb->fb->var.red.length   = 8;
		sfb->fb->var.green.length = 8;
		sfb->fb->var.blue.length  = 8;
		sfb->fb->var.red.offset   = 16;
		sfb->fb->var.green.offset = 8;
		sfb->fb->var.blue.offset  = 0;
		break;
	case 24:
		sfb->fb->fix.visual       = FB_VISUAL_TRUECOLOR;
		sfb->fb->fix.line_length  = sfb->fb->var.xres * 3;
		sfb->fb->var.red.length   = 8;
		sfb->fb->var.green.length = 8;
		sfb->fb->var.blue.length  = 8;
		sfb->fb->var.red.offset   = 16;
		sfb->fb->var.green.offset = 8;
		sfb->fb->var.blue.offset  = 0;
		break;
	case 8:
		sfb->fb->fix.visual       = FB_VISUAL_PSEUDOCOLOR;
		sfb->fb->fix.line_length  = sfb->fb->var.xres;
		sfb->fb->var.red.length   = 3;
		sfb->fb->var.green.length = 3;
		sfb->fb->var.blue.length  = 2;
		sfb->fb->var.red.offset   = 5;
		sfb->fb->var.green.offset = 2;
		sfb->fb->var.blue.offset  = 0;
		break;
	case 16:
	default:
		sfb->fb->fix.visual       = FB_VISUAL_TRUECOLOR;
		sfb->fb->fix.line_length  = sfb->fb->var.xres * 2;
		sfb->fb->var.red.length   = 5;
		sfb->fb->var.green.length = 6;
		sfb->fb->var.blue.length  = 5;
		sfb->fb->var.red.offset   = 11;
		sfb->fb->var.green.offset = 5;
		sfb->fb->var.blue.offset  = 0;
		break;
	}

	sfb->width  = sfb->fb->var.xres;
	sfb->height = sfb->fb->var.yres;
	sfb->hz = 60;
	smtc_set_timing(sfb);
}