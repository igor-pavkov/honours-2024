static int assess(const struct security_info *info, Table *overview_table, AnalyzeSecurityFlags flags) {
        static const struct {
                uint64_t exposure;
                const char *name;
                const char *color;
                SpecialGlyph smiley;
        } badness_table[] = {
                { 100, "DANGEROUS", ANSI_HIGHLIGHT_RED,    SPECIAL_GLYPH_DEPRESSED_SMILEY        },
                { 90,  "UNSAFE",    ANSI_HIGHLIGHT_RED,    SPECIAL_GLYPH_UNHAPPY_SMILEY          },
                { 75,  "EXPOSED",   ANSI_HIGHLIGHT_YELLOW, SPECIAL_GLYPH_SLIGHTLY_UNHAPPY_SMILEY },
                { 50,  "MEDIUM",    NULL,                  SPECIAL_GLYPH_NEUTRAL_SMILEY          },
                { 10,  "OK",        ANSI_HIGHLIGHT_GREEN,  SPECIAL_GLYPH_SLIGHTLY_HAPPY_SMILEY   },
                { 1,   "SAFE",      ANSI_HIGHLIGHT_GREEN,  SPECIAL_GLYPH_HAPPY_SMILEY            },
                { 0,   "PERFECT",   ANSI_HIGHLIGHT_GREEN,  SPECIAL_GLYPH_ECSTATIC_SMILEY         },
        };

        uint64_t badness_sum = 0, weight_sum = 0, exposure;
        _cleanup_(table_unrefp) Table *details_table = NULL;
        size_t i;
        int r;

        if (!FLAGS_SET(flags, ANALYZE_SECURITY_SHORT)) {
                details_table = table_new(" ", "name", "description", "weight", "badness", "range", "exposure");
                if (!details_table)
                        return log_oom();

                (void) table_set_sort(details_table, 3, 1, (size_t) -1);
                (void) table_set_reverse(details_table, 3, true);

                if (getenv_bool("SYSTEMD_ANALYZE_DEBUG") <= 0)
                        (void) table_set_display(details_table, 0, 1, 2, 6, (size_t) -1);
        }

        for (i = 0; i < ELEMENTSOF(security_assessor_table); i++) {
                const struct security_assessor *a = security_assessor_table + i;
                _cleanup_free_ char *d = NULL;
                uint64_t badness;
                void *data;

                data = (uint8_t*) info + a->offset;

                if (a->default_dependencies_only && !info->default_dependencies) {
                        badness = UINT64_MAX;
                        d = strdup("Service runs in special boot phase, option does not apply");
                        if (!d)
                                return log_oom();
                } else {
                        r = a->assess(a, info, data, &badness, &d);
                        if (r < 0)
                                return r;
                }

                assert(a->range > 0);

                if (badness != UINT64_MAX) {
                        assert(badness <= a->range);

                        badness_sum += DIV_ROUND_UP(badness * a->weight, a->range);
                        weight_sum += a->weight;
                }

                if (details_table) {
                        const char *checkmark, *description, *color = NULL;
                        TableCell *cell;

                        if (badness == UINT64_MAX) {
                                checkmark = " ";
                                description = a->description_na;
                                color = NULL;
                        } else if (badness == a->range) {
                                checkmark = special_glyph(SPECIAL_GLYPH_CROSS_MARK);
                                description = a->description_bad;
                                color = ansi_highlight_red();
                        } else if (badness == 0) {
                                checkmark = special_glyph(SPECIAL_GLYPH_CHECK_MARK);
                                description = a->description_good;
                                color = ansi_highlight_green();
                        } else {
                                checkmark = special_glyph(SPECIAL_GLYPH_CROSS_MARK);
                                description = NULL;
                                color = ansi_highlight_red();
                        }

                        if (d)
                                description = d;

                        r = table_add_cell_full(details_table, &cell, TABLE_STRING, checkmark, 1, 1, 0, 0, 0);
                        if (r < 0)
                                return log_error_errno(r, "Failed to add cell to table: %m");
                        if (color)
                                (void) table_set_color(details_table, cell, color);

                        r = table_add_cell(details_table, &cell, TABLE_STRING, a->id);
                        if (r < 0)
                                return log_error_errno(r, "Failed to add cell to table: %m");
                        if (a->url)
                                (void) table_set_url(details_table, cell, a->url);

                        r = table_add_cell(details_table, NULL, TABLE_STRING, description);
                        if (r < 0)
                                return log_error_errno(r, "Failed to add cell to table: %m");

                        r = table_add_cell(details_table, &cell, TABLE_UINT64, &a->weight);
                        if (r < 0)
                                return log_error_errno(r, "Failed to add cell to table: %m");
                        (void) table_set_align_percent(details_table, cell, 100);

                        r = table_add_cell(details_table, &cell, TABLE_UINT64, &badness);
                        if (r < 0)
                                return log_error_errno(r, "Failed to add cell to table: %m");
                        (void) table_set_align_percent(details_table, cell, 100);

                        r = table_add_cell(details_table, &cell, TABLE_UINT64, &a->range);
                        if (r < 0)
                                return log_error_errno(r, "Failed to add cell to table: %m");
                        (void) table_set_align_percent(details_table, cell, 100);

                        r = table_add_cell(details_table, &cell, TABLE_EMPTY, NULL);
                        if (r < 0)
                                return log_error_errno(r, "Failed to add cell to table: %m");
                        (void) table_set_align_percent(details_table, cell, 100);
                }
        }

        if (details_table) {
                size_t row;

                for (row = 1; row < table_get_rows(details_table); row++) {
                        char buf[DECIMAL_STR_MAX(uint64_t) + 1 + DECIMAL_STR_MAX(uint64_t) + 1];
                        const uint64_t *weight, *badness, *range;
                        TableCell *cell;
                        uint64_t x;

                        assert_se(weight = table_get_at(details_table, row, 3));
                        assert_se(badness = table_get_at(details_table, row, 4));
                        assert_se(range = table_get_at(details_table, row, 5));

                        if (*badness == UINT64_MAX || *badness == 0)
                                continue;

                        assert_se(cell = table_get_cell(details_table, row, 6));

                        x = DIV_ROUND_UP(DIV_ROUND_UP(*badness * *weight * 100U, *range), weight_sum);
                        xsprintf(buf, "%" PRIu64 ".%" PRIu64, x / 10, x % 10);

                        r = table_update(details_table, cell, TABLE_STRING, buf);
                        if (r < 0)
                                return log_error_errno(r, "Failed to update cell in table: %m");
                }

                r = table_print(details_table, stdout);
                if (r < 0)
                        return log_error_errno(r, "Failed to output table: %m");
        }

        assert(weight_sum > 0);
        exposure = DIV_ROUND_UP(badness_sum * 100U, weight_sum);

        for (i = 0; i < ELEMENTSOF(badness_table); i++)
                if (exposure >= badness_table[i].exposure)
                        break;

        assert(i < ELEMENTSOF(badness_table));

        if (details_table) {
                _cleanup_free_ char *clickable = NULL;
                const char *name;

                /* If we shall output the details table, also print the brief summary underneath */

                if (info->fragment_path) {
                        r = terminal_urlify_path(info->fragment_path, info->id, &clickable);
                        if (r < 0)
                                return log_oom();

                        name = clickable;
                } else
                        name = info->id;

                printf("\n%s %sOverall exposure level for %s%s: %s%" PRIu64 ".%" PRIu64 " %s%s %s\n",
                       special_glyph(SPECIAL_GLYPH_ARROW),
                       ansi_highlight(),
                       name,
                       ansi_normal(),
                       colors_enabled() ? strempty(badness_table[i].color) : "",
                       exposure / 10, exposure % 10,
                       badness_table[i].name,
                       ansi_normal(),
                       special_glyph(badness_table[i].smiley));
        }

        fflush(stdout);

        if (overview_table) {
                char buf[DECIMAL_STR_MAX(uint64_t) + 1 + DECIMAL_STR_MAX(uint64_t) + 1];
                TableCell *cell;

                r = table_add_cell(overview_table, &cell, TABLE_STRING, info->id);
                if (r < 0)
                        return log_error_errno(r, "Failed to add cell to table: %m");
                if (info->fragment_path) {
                        _cleanup_free_ char *url = NULL;

                        r = file_url_from_path(info->fragment_path, &url);
                        if (r < 0)
                                return log_error_errno(r, "Failed to generate URL from path: %m");

                        (void) table_set_url(overview_table, cell, url);
                }

                xsprintf(buf, "%" PRIu64 ".%" PRIu64, exposure / 10, exposure % 10);
                r = table_add_cell(overview_table, &cell, TABLE_STRING, buf);
                if (r < 0)
                        return log_error_errno(r, "Failed to add cell to table: %m");
                (void) table_set_align_percent(overview_table, cell, 100);

                r = table_add_cell(overview_table, &cell, TABLE_STRING, badness_table[i].name);
                if (r < 0)
                        return log_error_errno(r, "Failed to add cell to table: %m");
                (void) table_set_color(overview_table, cell, strempty(badness_table[i].color));

                r = table_add_cell(overview_table, NULL, TABLE_STRING, special_glyph(badness_table[i].smiley));
                if (r < 0)
                        return log_error_errno(r, "Failed to add cell to table: %m");
        }

        return 0;
}