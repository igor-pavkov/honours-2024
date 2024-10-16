static gif_result gif_initialise_frame(gif_animation *gif)
{
        int frame;
        gif_frame *temp_buf;

        const unsigned char *gif_data, *gif_end;
        ssize_t gif_bytes;
        unsigned int flags = 0;
        unsigned int width, height, offset_x, offset_y;
        ssize_t block_size, colour_table_size;
        bool first_image = true;
        gif_result return_value;
        bool premature_eof = false;

        /* Get the frame to decode and our data position */
        frame = gif->frame_count;

        /* Get our buffer position etc. */
        gif_data = (const unsigned char *)(gif->gif_data + gif->buffer_position);
        gif_end = (const unsigned char *)(gif->gif_data + gif->buffer_size);
        gif_bytes = (gif_end - gif_data);

        /* Check if we've finished */
        if ((gif_bytes > 0) && (gif_data[0] == GIF_TRAILER)) {
                return GIF_OK;
        }

        /* Check if there is enough data remaining. The shortest block of data
         * is a 4-byte comment extension + 1-byte block terminator + 1-byte gif
         * trailer
         */
        if (gif_bytes < 6) {
                return GIF_INSUFFICIENT_DATA;
        }

        /* We could theoretically get some junk data that gives us millions of
         * frames, so we ensure that we don't have a silly number
         */
        if (frame > 262143)
                return GIF_FRAME_DATA_ERROR;

        /* Get some memory to store our pointers in etc. */
        if ((int)gif->frame_holders <= frame) {
                /* Allocate more memory */
                temp_buf = (gif_frame *)realloc(gif->frames, (frame + 1) * sizeof(gif_frame));
                if (temp_buf == NULL) {
                        return GIF_INSUFFICIENT_MEMORY;
                }
                gif->frames = temp_buf;
                gif->frame_holders = frame + 1;
        }

        /* Store our frame pointer. We would do it when allocating except we
         * start off with one frame allocated so we can always use realloc.
         */
        gif->frames[frame].frame_pointer = gif->buffer_position;
        gif->frames[frame].display = false;
        gif->frames[frame].virgin = true;
        gif->frames[frame].disposal_method = 0;
        gif->frames[frame].transparency = false;
        gif->frames[frame].frame_delay = 0;
        gif->frames[frame].redraw_required = false;

        /* Invalidate any previous decoding we have of this frame */
        if (gif->decoded_frame == frame) {
                gif->decoded_frame = GIF_INVALID_FRAME;
        }

        /* We pretend to initialise the frames, but really we just skip over
         * all the data contained within. This is all basically a cut down
         * version of gif_decode_frame that doesn't have any of the LZW bits in
         * it.
         */

        /* Initialise any extensions */
        gif->buffer_position = gif_data - gif->gif_data;
        return_value = gif_initialise_frame_extensions(gif, frame);
        if (return_value != GIF_OK) {
                return return_value;
        }
        gif_data = (gif->gif_data + gif->buffer_position);
        gif_bytes = (gif_end - gif_data);

        /* Check if we've finished */
        if ((gif_bytes = (gif_end - gif_data)) < 1) {
                return GIF_INSUFFICIENT_FRAME_DATA;
        }

        if (gif_data[0] == GIF_TRAILER) {
                gif->buffer_position = (gif_data - gif->gif_data);
                gif->frame_count = frame + 1;
                return GIF_OK;
        }

        /* If we're not done, there should be an image descriptor */
        if (gif_data[0] != GIF_IMAGE_SEPARATOR) {
                return GIF_FRAME_DATA_ERROR;
        }

        /* Do some simple boundary checking */
        if (gif_bytes < 10) {
                return GIF_INSUFFICIENT_FRAME_DATA;
        }
        offset_x = gif_data[1] | (gif_data[2] << 8);
        offset_y = gif_data[3] | (gif_data[4] << 8);
        width = gif_data[5] | (gif_data[6] << 8);
        height = gif_data[7] | (gif_data[8] << 8);

        /* Set up the redraw characteristics. We have to check for extending
         * the area due to multi-image frames.
         */
        if (!first_image) {
                if (gif->frames[frame].redraw_x > offset_x) {
                        gif->frames[frame].redraw_width += (gif->frames[frame].redraw_x - offset_x);
                        gif->frames[frame].redraw_x = offset_x;
                }

                if (gif->frames[frame].redraw_y > offset_y) {
                        gif->frames[frame].redraw_height += (gif->frames[frame].redraw_y - offset_y);
                        gif->frames[frame].redraw_y = offset_y;
                }

                if ((offset_x + width) > (gif->frames[frame].redraw_x + gif->frames[frame].redraw_width)) {
                        gif->frames[frame].redraw_width = (offset_x + width) - gif->frames[frame].redraw_x;
                }

                if ((offset_y + height) > (gif->frames[frame].redraw_y + gif->frames[frame].redraw_height)) {
                        gif->frames[frame].redraw_height = (offset_y + height) - gif->frames[frame].redraw_y;
                }
        } else {
                first_image = false;
                gif->frames[frame].redraw_x = offset_x;
                gif->frames[frame].redraw_y = offset_y;
                gif->frames[frame].redraw_width = width;
                gif->frames[frame].redraw_height = height;
        }

        /* if we are clearing the background then we need to redraw enough to
         * cover the previous frame too
         */
        gif->frames[frame].redraw_required = ((gif->frames[frame].disposal_method == GIF_FRAME_CLEAR) ||
                                                (gif->frames[frame].disposal_method == GIF_FRAME_RESTORE));

        /* Boundary checking - shouldn't ever happen except with junk data */
        if (gif_initialise_sprite(gif, (offset_x + width), (offset_y + height))) {
                return GIF_INSUFFICIENT_MEMORY;
        }

        /* Decode the flags */
        flags = gif_data[9];
        colour_table_size = 2 << (flags & GIF_COLOUR_TABLE_SIZE_MASK);

        /* Move our data onwards and remember we've got a bit of this frame */
        gif_data += 10;
        gif_bytes = (gif_end - gif_data);
        gif->frame_count_partial = frame + 1;

        /* Skip the local colour table */
        if (flags & GIF_COLOUR_TABLE_MASK) {
                gif_data += 3 * colour_table_size;
                if ((gif_bytes = (gif_end - gif_data)) < 0) {
                        return GIF_INSUFFICIENT_FRAME_DATA;
                }
        }

        /* Ensure we have a correct code size */
        if (gif_bytes < 1) {
                return GIF_INSUFFICIENT_FRAME_DATA;
        }
        if (gif_data[0] > LZW_CODE_MAX) {
                return GIF_DATA_ERROR;
        }

        /* Move our pointer to the actual image data */
        gif_data++;
        --gif_bytes;

        /* Repeatedly skip blocks until we get a zero block or run out of data
         * These blocks of image data are processed later by gif_decode_frame()
         */
        block_size = 0;
        while (block_size != 1) {
                if (gif_bytes < 1) return GIF_INSUFFICIENT_FRAME_DATA;
                block_size = gif_data[0] + 1;
                /* Check if the frame data runs off the end of the file	*/
                if ((ssize_t)(gif_bytes - block_size) < 0) {
                        /* Try to recover by signaling the end of the gif.
                         * Once we get garbage data, there is no logical way to
                         * determine where the next frame is.  It's probably
                         * better to partially load the gif than not at all.
                         */
                        if (gif_bytes >= 2) {
                                gif_bytes = 1;
                                ++gif_data;
                                premature_eof = true;
                                break;
                        } else {
                                return GIF_INSUFFICIENT_FRAME_DATA;
                        }
                } else {
                        gif_bytes -= block_size;
                        gif_data += block_size;
                }
        }

        /* Add the frame and set the display flag */
        gif->buffer_position = gif_data - gif->gif_data;
        gif->frame_count = frame + 1;
        gif->frames[frame].display = true;

        /* Check if we've finished */
        if (gif_bytes < 1) {
                return GIF_INSUFFICIENT_FRAME_DATA;
        } else {
                if (premature_eof || gif_data[0] == GIF_TRAILER) {
                        return GIF_OK;
                }
        }
        return GIF_WORKING;
}