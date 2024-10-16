static int execute_code(AVCodecContext * avctx, int c)
{
    AnsiContext *s = avctx->priv_data;
    int ret, i;
    int width  = avctx->width;
    int height = avctx->height;

    switch(c) {
    case 'A': //Cursor Up
        s->y = FFMAX(s->y - (s->nb_args > 0 ? s->args[0]*s->font_height : s->font_height), 0);
        break;
    case 'B': //Cursor Down
        s->y = FFMIN(s->y + (s->nb_args > 0 ? s->args[0]*s->font_height : s->font_height), avctx->height - s->font_height);
        break;
    case 'C': //Cursor Right
        s->x = FFMIN(s->x + (s->nb_args > 0 ? s->args[0]*FONT_WIDTH : FONT_WIDTH), avctx->width  - FONT_WIDTH);
        break;
    case 'D': //Cursor Left
        s->x = FFMAX(s->x - (s->nb_args > 0 ? s->args[0]*FONT_WIDTH : FONT_WIDTH), 0);
        break;
    case 'H': //Cursor Position
    case 'f': //Horizontal and Vertical Position
        s->y = s->nb_args > 0 ? av_clip((s->args[0] - 1)*s->font_height, 0, avctx->height - s->font_height) : 0;
        s->x = s->nb_args > 1 ? av_clip((s->args[1] - 1)*FONT_WIDTH,     0, avctx->width  - FONT_WIDTH) : 0;
        break;
    case 'h': //set creen mode
    case 'l': //reset screen mode
        if (s->nb_args < 2)
            s->args[0] = DEFAULT_SCREEN_MODE;
        switch(s->args[0]) {
        case 0: case 1: case 4: case 5: case 13: case 19: //320x200 (25 rows)
            s->font = avpriv_cga_font;
            s->font_height = 8;
            width  = 40<<3;
            height = 25<<3;
            break;
        case 2: case 3: //640x400 (25 rows)
            s->font = avpriv_vga16_font;
            s->font_height = 16;
            width  = 80<<3;
            height = 25<<4;
            break;
        case 6: case 14: //640x200 (25 rows)
            s->font = avpriv_cga_font;
            s->font_height = 8;
            width  = 80<<3;
            height = 25<<3;
            break;
        case 7: //set line wrapping
            break;
        case 15: case 16: //640x350 (43 rows)
            s->font = avpriv_cga_font;
            s->font_height = 8;
            width  = 80<<3;
            height = 43<<3;
            break;
        case 17: case 18: //640x480 (60 rows)
            s->font = avpriv_cga_font;
            s->font_height = 8;
            width  = 80<<3;
            height = 60<<4;
            break;
        default:
            avpriv_request_sample(avctx, "Unsupported screen mode");
        }
        s->x = av_clip(s->x, 0, width  - FONT_WIDTH);
        s->y = av_clip(s->y, 0, height - s->font_height);
        if (width != avctx->width || height != avctx->height) {
            av_frame_unref(s->frame);
            ret = ff_set_dimensions(avctx, width, height);
            if (ret < 0)
                return ret;
            if ((ret = ff_get_buffer(avctx, s->frame,
                                     AV_GET_BUFFER_FLAG_REF)) < 0)
                return ret;
            s->frame->pict_type           = AV_PICTURE_TYPE_I;
            s->frame->palette_has_changed = 1;
            set_palette((uint32_t *)s->frame->data[1]);
            erase_screen(avctx);
        } else if (c == 'l') {
            erase_screen(avctx);
        }
        break;
    case 'J': //Erase in Page
        switch (s->args[0]) {
        case 0:
            erase_line(avctx, s->x, avctx->width - s->x);
            if (s->y < avctx->height - s->font_height)
                memset(s->frame->data[0] + (s->y + s->font_height)*s->frame->linesize[0],
                    DEFAULT_BG_COLOR, (avctx->height - s->y - s->font_height)*s->frame->linesize[0]);
            break;
        case 1:
            erase_line(avctx, 0, s->x);
            if (s->y > 0)
                memset(s->frame->data[0], DEFAULT_BG_COLOR, s->y * s->frame->linesize[0]);
            break;
        case 2:
            erase_screen(avctx);
        }
        break;
    case 'K': //Erase in Line
        switch(s->args[0]) {
        case 0:
            erase_line(avctx, s->x, avctx->width - s->x);
            break;
        case 1:
            erase_line(avctx, 0, s->x);
            break;
        case 2:
            erase_line(avctx, 0, avctx->width);
        }
        break;
    case 'm': //Select Graphics Rendition
        if (s->nb_args == 0) {
            s->nb_args = 1;
            s->args[0] = 0;
        }
        for (i = 0; i < FFMIN(s->nb_args, MAX_NB_ARGS); i++) {
            int m = s->args[i];
            if (m == 0) {
                s->attributes = 0;
                s->fg = DEFAULT_FG_COLOR;
                s->bg = DEFAULT_BG_COLOR;
            } else if (m == 1 || m == 2 || m == 4 || m == 5 || m == 7 || m == 8) {
                s->attributes |= 1 << (m - 1);
            } else if (m >= 30 && m <= 37) {
                s->fg = ansi_to_cga[m - 30];
            } else if (m == 38 && i + 2 < FFMIN(s->nb_args, MAX_NB_ARGS) && s->args[i + 1] == 5 && s->args[i + 2] < 256) {
                int index = s->args[i + 2];
                s->fg = index < 16 ? ansi_to_cga[index] : index;
                i += 2;
            } else if (m == 39) {
                s->fg = ansi_to_cga[DEFAULT_FG_COLOR];
            } else if (m >= 40 && m <= 47) {
                s->bg = ansi_to_cga[m - 40];
            } else if (m == 48 && i + 2 < FFMIN(s->nb_args, MAX_NB_ARGS) && s->args[i + 1] == 5 && s->args[i + 2] < 256) {
                int index = s->args[i + 2];
                s->bg = index < 16 ? ansi_to_cga[index] : index;
                i += 2;
            } else if (m == 49) {
                s->fg = ansi_to_cga[DEFAULT_BG_COLOR];
            } else {
                avpriv_request_sample(avctx, "Unsupported rendition parameter");
            }
        }
        break;
    case 'n': //Device Status Report
    case 'R': //report current line and column
        /* ignore */
        break;
    case 's': //Save Cursor Position
        s->sx = s->x;
        s->sy = s->y;
        break;
    case 'u': //Restore Cursor Position
        s->x = av_clip(s->sx, 0, avctx->width  - FONT_WIDTH);
        s->y = av_clip(s->sy, 0, avctx->height - s->font_height);
        break;
    default:
        avpriv_request_sample(avctx, "Unknown escape code");
        break;
    }
    s->x = av_clip(s->x, 0, avctx->width  - FONT_WIDTH);
    s->y = av_clip(s->y, 0, avctx->height - s->font_height);
    return 0;
}