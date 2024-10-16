static gif_result gif_skip_frame_extensions(gif_animation *gif)
{
        const unsigned char *gif_data, *gif_end;
        ssize_t gif_bytes;
        ssize_t block_size;

        /* Get our buffer position etc.	*/
        gif_data = (const unsigned char *)(gif->gif_data + gif->buffer_position);
        gif_end = (const unsigned char *)(gif->gif_data + gif->buffer_size);
        gif_bytes = (gif_end - gif_data);

        /* Skip the extensions */
        while (gif_data < gif_end && gif_data[0] == GIF_EXTENSION_INTRODUCER) {
                ++gif_data;
                if (gif_data >= gif_end) {
                        return GIF_INSUFFICIENT_FRAME_DATA;
                }

                /* Switch on extension label */
                switch(gif_data[0]) {
                case GIF_EXTENSION_COMMENT:
                        /* Move the pointer to the first data sub-block
                         * 1 byte for the extension label
                         */
                        ++gif_data;
                        break;

                default:
                        /* Move the pointer to the first data sub-block 2 bytes
                         * for the extension label and size fields Skip the
                         * extension size itself
                         */
                        if (gif_data + 1 >= gif_end) {
                                return GIF_INSUFFICIENT_FRAME_DATA;
                        }
                        gif_data += (2 + gif_data[1]);
                }

                /* Repeatedly skip blocks until we get a zero block or run out
                 * of data This data is ignored by this gif decoder
                 */
                gif_bytes = (gif_end - gif_data);
                block_size = 0;
                while (gif_data < gif_end && gif_data[0] != GIF_BLOCK_TERMINATOR) {
                        block_size = gif_data[0] + 1;
                        if ((gif_bytes -= block_size) < 0) {
                                return GIF_INSUFFICIENT_FRAME_DATA;
                        }
                        gif_data += block_size;
                }
                ++gif_data;
        }

        /* Set buffer position and return */
        gif->buffer_position = (gif_data - gif->gif_data);
        return GIF_OK;
}