static uint16_t tsc2102_audio_register_read(TSC210xState *s, int reg)
{
    int l_ch, r_ch;
    uint16_t val;

    switch (reg) {
    case 0x00:	/* Audio Control 1 */
        return s->audio_ctrl1;

    case 0x01:
        return 0xff00;

    case 0x02:	/* DAC Volume Control */
        return s->volume;

    case 0x03:
        return 0x8b00;

    case 0x04:	/* Audio Control 2 */
        l_ch = 1;
        r_ch = 1;
        if (s->softstep && !(s->dac_power & (1 << 10))) {
            l_ch = (qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) >
                            s->volume_change + TSC_SOFTSTEP_DELAY);
            r_ch = (qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) >
                            s->volume_change + TSC_SOFTSTEP_DELAY);
        }

        return s->audio_ctrl2 | (l_ch << 3) | (r_ch << 2);

    case 0x05:	/* Stereo DAC Power Control */
        return 0x2aa0 | s->dac_power |
                (((s->dac_power & (1 << 10)) &&
                  (qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL) >
                   s->powerdown + TSC_POWEROFF_DELAY)) << 6);

    case 0x06:	/* Audio Control 3 */
        val = s->audio_ctrl3 | 0x0001;
        s->audio_ctrl3 &= 0xff3f;
        return val;

    case 0x07:	/* LCH_BASS_BOOST_N0 */
    case 0x08:	/* LCH_BASS_BOOST_N1 */
    case 0x09:	/* LCH_BASS_BOOST_N2 */
    case 0x0a:	/* LCH_BASS_BOOST_N3 */
    case 0x0b:	/* LCH_BASS_BOOST_N4 */
    case 0x0c:	/* LCH_BASS_BOOST_N5 */
    case 0x0d:	/* LCH_BASS_BOOST_D1 */
    case 0x0e:	/* LCH_BASS_BOOST_D2 */
    case 0x0f:	/* LCH_BASS_BOOST_D4 */
    case 0x10:	/* LCH_BASS_BOOST_D5 */
    case 0x11:	/* RCH_BASS_BOOST_N0 */
    case 0x12:	/* RCH_BASS_BOOST_N1 */
    case 0x13:	/* RCH_BASS_BOOST_N2 */
    case 0x14:	/* RCH_BASS_BOOST_N3 */
    case 0x15:	/* RCH_BASS_BOOST_N4 */
    case 0x16:	/* RCH_BASS_BOOST_N5 */
    case 0x17:	/* RCH_BASS_BOOST_D1 */
    case 0x18:	/* RCH_BASS_BOOST_D2 */
    case 0x19:	/* RCH_BASS_BOOST_D4 */
    case 0x1a:	/* RCH_BASS_BOOST_D5 */
        return s->filter_data[reg - 0x07];

    case 0x1b:	/* PLL Programmability 1 */
        return s->pll[0];

    case 0x1c:	/* PLL Programmability 2 */
        return s->pll[1];

    case 0x1d:	/* Audio Control 4 */
        return (!s->softstep) << 14;

    default:
#ifdef TSC_VERBOSE
        fprintf(stderr, "tsc2102_audio_register_read: "
                        "no such register: 0x%02x\n", reg);
#endif
        return 0xffff;
    }
}