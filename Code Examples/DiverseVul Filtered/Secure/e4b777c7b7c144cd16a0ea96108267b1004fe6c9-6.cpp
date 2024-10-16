gif_result gif_initialise(gif_animation *gif, size_t size, unsigned char *data)
{
        const unsigned char *gif_data;
        unsigned int index;
        gif_result return_value;

        /* Initialize values */
        gif->buffer_size = size;
        gif->gif_data = data;

        if (gif->lzw_ctx == NULL) {
                lzw_result res = lzw_context_create(
                                (struct lzw_ctx **)&gif->lzw_ctx);
                if (res != LZW_OK) {
                        return gif_error_from_lzw(res);
                }
        }

        /* Check for sufficient data to be a GIF (6-byte header + 7-byte
         * logical screen descriptor)
         */
        if (gif->buffer_size < GIF_STANDARD_HEADER_SIZE) {
                return GIF_INSUFFICIENT_DATA;
        }

        /* Get our current processing position */
        gif_data = gif->gif_data + gif->buffer_position;

        /* See if we should initialise the GIF */
        if (gif->buffer_position == 0) {
                /* We want everything to be NULL before we start so we've no
                 * chance of freeing bad pointers (paranoia)
                 */
                gif->frame_image = NULL;
                gif->frames = NULL;
                gif->local_colour_table = NULL;
                gif->global_colour_table = NULL;

                /* The caller may have been lazy and not reset any values */
                gif->frame_count = 0;
                gif->frame_count_partial = 0;
                gif->decoded_frame = GIF_INVALID_FRAME;

                /* 6-byte GIF file header is:
                 *
                 *	+0	3CHARS	Signature ('GIF')
                 *	+3	3CHARS	Version ('87a' or '89a')
                 */
                if (strncmp((const char *) gif_data, "GIF", 3) != 0) {
                        return GIF_DATA_ERROR;
                }
                gif_data += 3;

                /* Ensure GIF reports version 87a or 89a */
                /*
                if ((strncmp(gif_data, "87a", 3) != 0) &&
                    (strncmp(gif_data, "89a", 3) != 0))
                               LOG(("Unknown GIF format - proceeding anyway"));
                */
                gif_data += 3;

                /* 7-byte Logical Screen Descriptor is:
                 *
                 *	+0	SHORT	Logical Screen Width
                 *	+2	SHORT	Logical Screen Height
                 *	+4	CHAR	__Packed Fields__
                 *                      1BIT	Global Colour Table Flag
                 *                      3BITS	Colour Resolution
                 *                      1BIT	Sort Flag
                 *                      3BITS	Size of Global Colour Table
                 *	+5	CHAR	Background Colour Index
                 *	+6	CHAR	Pixel Aspect Ratio
                 */
                gif->width = gif_data[0] | (gif_data[1] << 8);
                gif->height = gif_data[2] | (gif_data[3] << 8);
                gif->global_colours = (gif_data[4] & GIF_COLOUR_TABLE_MASK);
                gif->colour_table_size = (2 << (gif_data[4] & GIF_COLOUR_TABLE_SIZE_MASK));
                gif->background_index = gif_data[5];
                gif->aspect_ratio = gif_data[6];
                gif->loop_count = 1;
                gif_data += 7;

                /* Some broken GIFs report the size as the screen size they
                 * were created in. As such, we detect for the common cases and
                 * set the sizes as 0 if they are found which results in the
                 * GIF being the maximum size of the frames.
                 */
                if (((gif->width == 640) && (gif->height == 480)) ||
                    ((gif->width == 640) && (gif->height == 512)) ||
                    ((gif->width == 800) && (gif->height == 600)) ||
                    ((gif->width == 1024) && (gif->height == 768)) ||
                    ((gif->width == 1280) && (gif->height == 1024)) ||
                    ((gif->width == 1600) && (gif->height == 1200)) ||
                    ((gif->width == 0) || (gif->height == 0)) ||
                    ((gif->width > 2048) || (gif->height > 2048))) {
                        gif->width = 1;
                        gif->height = 1;
                }

                /* Allocate some data irrespective of whether we've got any
                 * colour tables. We always get the maximum size in case a GIF
                 * is lying to us. It's far better to give the wrong colours
                 * than to trample over some memory somewhere.
                */
                gif->global_colour_table = calloc(GIF_MAX_COLOURS, sizeof(unsigned int));
                gif->local_colour_table = calloc(GIF_MAX_COLOURS, sizeof(unsigned int));
                if ((gif->global_colour_table == NULL) ||
                    (gif->local_colour_table == NULL)) {
                        gif_finalise(gif);
                        return GIF_INSUFFICIENT_MEMORY;
                }

                /* Set the first colour to a value that will never occur in
                 * reality so we know if we've processed it
                */
                gif->global_colour_table[0] = GIF_PROCESS_COLOURS;

                /* Check if the GIF has no frame data (13-byte header + 1-byte
                 * termination block) Although generally useless, the GIF
                 * specification does not expressly prohibit this
                 */
                if (gif->buffer_size == (GIF_STANDARD_HEADER_SIZE + 1)) {
                        if (gif_data[0] == GIF_TRAILER) {
                                return GIF_OK;
                        } else {
                                return GIF_INSUFFICIENT_DATA;
                        }
                }

                /* Initialise enough workspace for a frame */
                if ((gif->frames = (gif_frame *)malloc(sizeof(gif_frame))) == NULL) {
                        gif_finalise(gif);
                        return GIF_INSUFFICIENT_MEMORY;
                }
                gif->frame_holders = 1;

                /* Initialise the bitmap header */
                assert(gif->bitmap_callbacks.bitmap_create);
                gif->frame_image = gif->bitmap_callbacks.bitmap_create(gif->width, gif->height);
                if (gif->frame_image == NULL) {
                        gif_finalise(gif);
                        return GIF_INSUFFICIENT_MEMORY;
                }

                /* Remember we've done this now */
                gif->buffer_position = gif_data - gif->gif_data;
        }

        /*  Do the colour map if we haven't already. As the top byte is always
         *  0xff or 0x00 depending on the transparency we know if it's been
         *  filled in.
         */
        if (gif->global_colour_table[0] == GIF_PROCESS_COLOURS) {
                /* Check for a global colour map signified by bit 7 */
                if (gif->global_colours) {
                        if (gif->buffer_size < (gif->colour_table_size * 3 + GIF_STANDARD_HEADER_SIZE)) {
                                return GIF_INSUFFICIENT_DATA;
                        }
                        for (index = 0; index < gif->colour_table_size; index++) {
                                /* Gif colour map contents are r,g,b.
                                 *
                                 * We want to pack them bytewise into the
                                 * colour table, such that the red component
                                 * is in byte 0 and the alpha component is in
                                 * byte 3.
                                 */
                                unsigned char *entry = (unsigned char *) &gif->
                                                       global_colour_table[index];

                                entry[0] = gif_data[0];	/* r */
                                entry[1] = gif_data[1];	/* g */
                                entry[2] = gif_data[2];	/* b */
                                entry[3] = 0xff;	/* a */

                                gif_data += 3;
                        }
                        gif->buffer_position = (gif_data - gif->gif_data);
                } else {
                        /* Create a default colour table with the first two
                         * colours as black and white
                         */
                        unsigned int *entry = gif->global_colour_table;

                        entry[0] = 0x00000000;
                        /* Force Alpha channel to opaque */
                        ((unsigned char *) entry)[3] = 0xff;

                        entry[1] = 0xffffffff;
                }
        }

        /* Repeatedly try to initialise frames */
        while ((return_value = gif_initialise_frame(gif)) == GIF_WORKING);

        /* If there was a memory error tell the caller */
        if ((return_value == GIF_INSUFFICIENT_MEMORY) ||
            (return_value == GIF_DATA_ERROR)) {
                return return_value;
        }

        /* If we didn't have some frames then a GIF_INSUFFICIENT_DATA becomes a
         * GIF_INSUFFICIENT_FRAME_DATA
         */
        if ((return_value == GIF_INSUFFICIENT_DATA) &&
            (gif->frame_count_partial > 0)) {
                return GIF_INSUFFICIENT_FRAME_DATA;
        }

        /* Return how many we got */
        return return_value;
}