static void tsc2102_data_register_write(
                TSC210xState *s, int reg, uint16_t value)
{
    switch (reg) {
    case 0x00:	/* X */
    case 0x01:	/* Y */
    case 0x02:	/* Z1 */
    case 0x03:	/* Z2 */
    case 0x05:	/* BAT1 */
    case 0x06:	/* BAT2 */
    case 0x07:	/* AUX1 */
    case 0x08:	/* AUX2 */
    case 0x09:	/* TEMP1 */
    case 0x0a:	/* TEMP2 */
        return;

    default:
#ifdef TSC_VERBOSE
        fprintf(stderr, "tsc2102_data_register_write: "
                        "no such register: 0x%02x\n", reg);
#endif
    }
}