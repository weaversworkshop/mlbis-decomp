// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "itcm/cpu_mem.h"

#include "main/mesWriter.h"

// --- Struct Definitions --- //

typedef union {
    u32 raw;  // +0x0
    struct {
        u32 height : 4;  // +0x0
        u32 width : 4;
        u32 : 16;
        u32 widthWords : 8;
    };
} MesGlyphHeader;

// --- Variable Definitions --- //

const u32 mesGlyphRowMaskLo[4] = { 0x00001111, 0x00002222, 0x00004444, 0x00008888 };  // 0x0204a4dc
const u32 mesGlyphRowMask[4] = { 0x11111111, 0x22222222, 0x44444444, 0x88888888 };  // 0x0204a4cc
const u32 mesGlyphRowMaskHi[4] = { 0x11110000, 0x22220000, 0x44440000, 0x88880000 };  // 0x0204a4ec

// --- Function Declarations --- //

u16 mesWriterLineWidth(MesWriter *);  // 0x0200bc48
u32 mesWriterExecCode(MesWriter *, MesStepResult *, BOOL);  // 0x0200c4b8

void mesGlyphBlitTile(u32 *, u16, u16, u16, u32 *, u8, u8);  // 0x0200cb30
void mesGlyphBlitRow(u32 *, s32, u16, u16, u32 *, u8, u8);  // 0x0200ce04
void mesGlyphBlitTileScaled(u32 *, u16, u16, s32, u16, u32 *, u8, u8);  // 0x0200d0d8
void mesGlyphBlitRowScaled(u32 *, s32, u16, s32, u16, u32 *, u8, u8);  // 0x0200d63c

// --- Function Definitions --- //

// MATCHED 100%
void mesWriterInit(MesWriter * writer, MesFont ** fonts, u32 * dst, const char * text, u8 x, u8 y, u8 spacing, u8 lineSpacing, u8 color, u8 spaceWidth, u8 cols, u8 width, u8 xOffset, u8 mode, MesCodeHandler * callbacks) {
    writer->fonts = fonts;
    writer->dst = dst;
    writer->text = text;
    writer->x = x;
    writer->y = y;
    writer->baseSpacing = spacing;
    writer->spacing = spacing;
    writer->lineSpacing = lineSpacing;
    writer->baseColor = color;
    writer->color = color;
    writer->spaceWidth = spaceWidth;
    writer->cols = cols;
    writer->xOffset = xOffset;
    writer->mode = mode;

    if (width == 0) {
        writer->width = cols;
    } else {
        writer->width = width;
    }

    writer->font = 0;
    writer->scale = 0;
    writer->callbacks = callbacks;

    mesWriterReset(writer);
}

// MATCHED 100%
void mesWriterReset(MesWriter * writer) {
    writer->lineHeight = 0;
    writer->align = 0;
    writer->noAlign = 0;
    writer->lastAlign = 0;
    writer->fixedWidth = 0;
    writer->lineX = writer->x;
    writer->field1c_0 = writer->y;
    writer->lastX = 0;
    writer->lastY = 0;
    writer->lineLeft = 0x3ff;
    writer->extents.lines = 1;
    writer->extents.width = 0;
    writer->extents.height = 0;
    writer->extents.left = 0x3ff;
    writer->extents.top = writer->y;
}

// MATCHED 100%
void mesWriterSetCallback(MesCodeHandler * callbacks, u32 code, u8 argLen, MesCodeCallback func, void * arg) {
    u16 idx = code - 1;

    callbacks[idx].argLen = argLen;
    callbacks[idx].func = func;
    callbacks[idx].arg = arg;
}

// MATCHED 100%
void mesWriterMeasure(MesWriter * writer, MesExtents * out) {
    MesWriter copy;

    if (writer < &copy) {
        cpu_copy32_rev(writer, &copy, sizeof(copy));
    } else {
        cpu_copy32_fwd(writer, &copy, sizeof(copy));
    }

    while (copy.text[0] != '\0') {
        mesWriterStep(&copy, TRUE, TRUE);
    }

    if (&copy.extents < out) {
        cpu_copy32_rev(&copy.extents, out, sizeof(MesExtents));
    } else {
        cpu_copy32_fwd(&copy.extents, out, sizeof(MesExtents));
    }
}

// MATCHED 100%
u16 mesWriterMeasureHeight(MesWriter * writer) {
    MesWriter copy;

    if (writer < &copy) {
        cpu_copy32_rev(writer, &copy, sizeof(copy));
    } else {
        cpu_copy32_fwd(writer, &copy, sizeof(copy));
    }

    copy.noAlign = 1;

    while (copy.text[0] != '\0') {
        mesWriterStep(&copy, TRUE, TRUE);
    }

    return copy.extents.height + (copy.extents.lines << 8);
}

// MATCHED 100%
u16 mesWriterLineWidth(MesWriter * writer) {
    MesWriter copy;

    if (writer < &copy) {
        cpu_copy32_rev(writer, &copy, sizeof(copy));
    } else {
        cpu_copy32_fwd(writer, &copy, sizeof(copy));
    }

    copy.noAlign = 1;

    while (TRUE) {
        if ((copy.text[0] == 0xff && copy.text[1] == 0) || copy.text[0] == '\0') {
            break;
        }

        mesWriterStep(&copy, TRUE, TRUE);
    }

    return copy.x - writer->x;
}

// MATCHED 100%
MesStepResult mesWriterStep(MesWriter * writer, BOOL skipDraw, BOOL measuring) {
    MesStepResult result = {0};
    MesGlyphSizeSlot size;
    MesGlyphSize sizeCopy[4];

    switch (writer->text[0]) {
        case '\0':
            break;
        case ' ': {
            u8 width = writer->spaceWidth - 1;

            writer->lastX = writer->x;
            writer->lastY = writer->y;

            if (writer->lineLeft == 0x3ff) {
                writer->lineLeft = writer->x;
            }

            writer->x += width + 1 + writer->spacing;
            writer->lastWidth = width;

            writer->text++;

            result.glyph = 0xff20;
            result.code = 0xff20;

            MesFont * font = writer->fonts[(u16)writer->font * 2];
            u32 header = MES_FONT_WORD(font, font->pageOffsets[0] / 4);

            width = (header & 0xf) * 4;

            if (width > writer->lineHeight) {
                writer->lineHeight = width;
            }
            break;
        }
        case 0xff: {
            writer->text += mesWriterExecCode(writer, &result, measuring) + 2;
            result.code = result.glyph;
            break;
        }
        default: {
            result.text = writer->text;

            writer->lastY = writer->y;

            MesFont ** fonts = writer->fonts;
            u16 fontIdx = writer->font;
            u16 ch = writer->text[0];
            MesFont * font;

            if (ch < 0x100) {
                font = fonts[fontIdx * 2];
            } else {
                font = fonts[fontIdx * 2 + 1];
                ch -= 0x100;
            }

            s32 glyph = ((u16 *)MES_FONT_PTR(font, font->charMapOffset))[ch];

            result.glyph = glyph;
            result.code = writer->text[0];

            if (!measuring && !skipDraw) {
                BOOL fixedWidth = writer->fixedWidth != 0;
                u16 fontIdx = writer->font;

                MesFont ** fonts = writer->fonts;
                MesFont * font;

                if (result.code < 0x100) {
                    font = fonts[fontIdx * 2];
                } else {
                    font = fonts[fontIdx * 2 + 1];
                }

                u32 * page = MES_FONT_PTR(font, MES_FONT_WORD(font, (u16)(glyph & 0xff) + 2));

                sizeCopy[0].raw = mesWriterDrawGlyph(writer->x, writer->y, writer->scale, glyph >> 8, (u16)writer->cols, writer->dst, page, writer->color, fixedWidth, writer->mode);
                sizeCopy[3].raw = sizeCopy[0].raw;
                size.raw = sizeCopy[0].raw;
            } else {
                BOOL fixedWidth = writer->fixedWidth != 0;
                u16 fontIdx = writer->font;

                MesFont ** fonts = writer->fonts;
                MesFont * font;

                if (result.code < 0x100) {
                    font = fonts[fontIdx * 2];
                } else {
                    font = fonts[fontIdx * 2 + 1];
                }

                u32 * page = MES_FONT_PTR(font, MES_FONT_WORD(font, (u8)glyph + 2));
                u8 scale = writer->scale;

                u32 width;
                u32 header;
                if (fixedWidth) {
                    header = page[0];
                    width = ((header & 0xf0) >> 4) * 4 - 1;
                } else {
                    u8 glyphIdx = glyph >> 8;
                    u8 word = (glyphIdx >> 3) + 1;
                    u8 shift = (glyphIdx & 7) * 4;

                    header = page[0];
                    width = (u8)((page[word] >> shift) & 0xf);

                    if (((header & 0xf0) >> 4) >= 5) {
                        u8 wordHi = word + ((header & 0xff000000) >> 25);
                        width = (u8)(width | ((page[wordHi] >> shift) << 4));
                    }
                }

                sizeCopy[1].width = width;
                sizeCopy[1].height = (header & 0xf) * 4 - 1;

                if (scale & 1) {
                    sizeCopy[1].width = (sizeCopy[1].width + 1) * 2 - 1;
                }

                if (scale & 2) {
                    sizeCopy[1].height = (sizeCopy[1].height + 1) * 2 - 1;
                }

                sizeCopy[2].raw = sizeCopy[1].raw;
                size.raw = sizeCopy[1].raw;
            }

            writer->lastX = writer->x;
            writer->lastY = writer->y;

            if (writer->lineLeft == 0x3ff) {
                writer->lineLeft = writer->x;
            }

            writer->x += size.width + 1 + writer->spacing;
            writer->lastWidth = size.width;
            writer->lastHeight = size.height;

            size.raw += 0x10001;

            if (size.height > writer->lineHeight) {
                writer->lineHeight = size.height;
            }

            writer->text++;

            if (writer->extents.left > writer->lineLeft) {
                writer->extents.left = writer->lineLeft;
            }

            if (writer->x - writer->extents.left > writer->extents.width) {
                writer->extents.width = writer->x - writer->extents.left;
            }

            writer->extents.height = writer->y - writer->extents.top + writer->lineHeight;
            break;
        }
    }

    return result;
}

// MATCHED 100%
s16 mesWriterDrawGlyph(u16 x, u16 y, u8 scale, u8 glyph, u32 cols, u32 * dst, u32 * page, u8 color, BOOL fixedWidth, u8 mode) {
    u16 stride = cols + 1;
    u8 size[2];

    switch (mode) {
        case 0:
            if (scale == 0) {
                mesGlyphBlitTile(dst, x, y, stride, page, glyph, color);
            } else {
                mesGlyphBlitTileScaled(dst, x, y, scale, stride, page, glyph, color);
            }
            break;
        case 1:
            if (scale == 0) {
                mesGlyphBlitRow(dst, x, y, stride, page, glyph, color);
            } else {
                mesGlyphBlitRowScaled(dst, x, y, scale, stride, page, glyph, color);
            }
            break;
    }

    u32 width;
    u32 header;

    if (fixedWidth) {
        header = page[0];
        width = ((header & 0xf0) >> 4) * 4 - 1;
    } else {
        u8 word = (glyph >> 3) + 1;
        u8 shift = (glyph & 7) * 4;

        header = page[0];
        width = (u8)((page[word] >> shift) & 0xf);

        if (((header & 0xf0) >> 4) >= 5) {
            u8 wordHi = word + ((header & 0xff000000) >> 25);
            width = (u8)(width | ((page[wordHi] >> shift) << 4));
        }
    }

    size[0] = width;
    size[1] = (header & 0xf) * 4 - 1;

    if (scale & 1) {
        size[0] = (size[0] + 1) * 2 - 1;
    }

    if (scale & 2) {
        size[1] = (size[1] + 1) * 2 - 1;
    }

    return *(s16 *)size;
}

// MATCHED 100%
u32 mesWriterExecCode(MesWriter * writer, MesStepResult * result, BOOL measuring) {
    u8 cmd = writer->text[1];
    u32 argLen = 0;

    if (cmd == 0) {
        writer->extents.lines++;
        writer->y += writer->lineHeight + writer->lineSpacing;
        writer->x = writer->lineX;
        writer->lineHeight = 0;
        writer->lineLeft = 0x3ff;

        if (writer->noAlign == 0) {
            writer->text += 2;

            switch (writer->align) {
                case 1:
                    writer->x = (writer->width + 1) * 4 - (mesWriterLineWidth(writer) >> 1);
                    writer->x += writer->xOffset;
                    break;
                case 2:
                    writer->x = (writer->width + 1) * 8 - mesWriterLineWidth(writer);
                    writer->x -= writer->lineX - writer->xOffset;
                    writer->x += writer->xOffset;
                    break;
            }

            writer->text -= 2;
        }

        result->glyph = 0xff00;
    } else if (cmd < 0x20) {
        MesCodeHandler * handler;

        if (writer->callbacks == NULL) {
            return argLen;
        }

        handler = &writer->callbacks[cmd - 1];

        if (handler->func != NULL && handler->disabled == 0) {
            if (handler->argLen < 0) {
                argLen = handler->func(handler->arg, writer, measuring);
            } else {
                handler->func(handler->arg, writer, measuring);
                argLen = handler->argLen;
            }
        } else {
            argLen = handler->argLen;
        }

        if (!measuring) {
            result->glyph = (cmd & 0x1f) | 0xff00;
            result->text = writer->text + 2;
        }
    } else if (cmd < 0x30) {
        if (!measuring) {
            writer->color = (cmd & 0xf) + writer->baseColor;

            if (writer->color > 14) {
                writer->color = 14;
            }
        }
    } else if (cmd < 0x40) {
        u32 sub = cmd & 0xf;

        if (sub < 4) {
            if ((sub & 3) == 0) {
                writer->scaleWide = 0;
                writer->scaleTall = 0;
            } else {
                if (sub & 1) {
                    writer->scaleTall = 1;
                }
                if (sub & 2) {
                    writer->scaleWide = 1;
                }
            }
        } else if (sub < 7) {
            if (writer->noAlign == 0) {
                u8 align = sub - 4;

                if (writer->lastAlign != align) {
                    writer->lastAlign = align;
                    writer->align = align;

                    switch (writer->align) {
                        case 1:
                            writer->x = (writer->width + 1) * 4 - (mesWriterLineWidth(writer) >> 1);
                            writer->x += writer->xOffset;
                            break;
                        case 2:
                            writer->x = (writer->width + 1) * 8 - mesWriterLineWidth(writer);
                            writer->x -= writer->lineX - writer->xOffset;
                            writer->x += writer->xOffset;
                            break;
                    }

                    result->glyph = writer->align + 0xff30;
                    result->code = writer->align + 0xff30;
                }
            }
        } else if (sub < 9) {
            writer->fixedWidth = (s32)(sub & 8) >> 3;
        }
    } else if (cmd < 0x50) {
        writer->font = cmd & 0xf;
    } else if (cmd < 0x60) {
        writer->spaceWidth = cmd & 0xf;
    } else if (cmd < 0x80) {
        writer->x += cmd - 0x5f;
    } else if (cmd < 0xa0) {
        writer->x -= cmd - 0x7f;
    } else if (cmd < 0xc0) {
        writer->y += cmd - 0x9f;
    } else if (cmd < 0xe0) {
        writer->y -= cmd - 0xbf;
    } else if (cmd < 0xe8) {
        u8 spacing = cmd - 0xe0;
        writer->baseSpacing = spacing;
        writer->spacing = spacing;
    } else if (cmd < 0xf0) {
        u8 spacing = cmd - 0xe8;

        if (spacing == 7) {
            writer->spacing = writer->baseSpacing;
        } else {
            writer->spacing = spacing;
        }
    }

    return argLen;
}

#pragma optimization_level 2
#pragma opt_common_subs off

// MATCHED 100%
void mesGlyphBlitTile(u32 * dst, u16 x, u16 y, u16 stride, u32 * page, u8 glyph, u8 color) {
    s16 row;
    s16 colsLeft;
    u32 * dstCol;
    s16 bit;
    u16 yEnd;
    u32 shr;
    u32 color2;
    s16 row0;
    u32 color1;
    u32 shl;
    u32 inv;
    u16 widthPx;
    s16 bit2;
    s16 row2;
    MesGlyphHeader header;

    header.raw = page[0];

    u32 cellW = (u16)((header.raw & 0xf0) >> 4);
    u16 cellH = header.raw & 15;
    page = page + 1 + ((header.raw & 0xff000000) >> 24);
    u32 cellWords = glyph * cellW;
    u32 skip = cellH * cellWords;
    shl = (u16)((x & 7) << 2);
    shr = (u16)(32 - shl);
    page += skip;
    dstCol = dst + ((x & 0xfff8) + (y & 0xfff8) * stride);
    y &= 7;
    yEnd = y + cellH * 4;
    widthPx = cellW * 4;
    colsLeft = widthPx & 0xfff8;

    if (colsLeft > 0) {
        color1 = color;
        row0 = (s16)y;

        do {
            row = row0;

            if (row < yEnd) {
                inv = 0xffffffff;

                do {
                    bit = 0;

                    do {
                        u16 idx = (row & 0xfff8) * stride + (row & 7);
                        row++;
                        u32 mask = mesGlyphRowMask[bit];
                        u32 plane0 = (page[0] & mask) >> bit;
                        u32 * q = dstCol + idx;
                        u32 base = plane0 * color1;
                        u32 clear = plane0 * 16 - plane0;
                        dstCol[idx] &= inv ^ (clear << shl);
                        q[8] &= inv ^ (clear >> shr);
                        u32 pix = base + ((page[1] & mask) >> bit);
                        dstCol[idx] += pix << shl;
                        q[8] += pix >> shr;

                        bit++;
                    } while (bit < 4);

                    page += 2;
                } while (row < yEnd);
            }

            colsLeft -= 8;
            dstCol += 8;
        } while (colsLeft > 0);
    }

    if (widthPx & 4) {
        row2 = (s16)y;

        if (row2 < yEnd) {
            color2 = color;

            do {
                inv = 0xffffffff;
                bit2 = 0;

                do {
                    u16 idx = (row2 & 0xfff8) * stride + (row2 & 7);
                    row2++;
                    u32 plane0 = (page[0] & mesGlyphRowMaskLo[bit2]) >> bit2;
                    u32 * q = dstCol + idx;
                    u32 clear = plane0 * 16 - plane0;
                    dstCol[idx] &= inv ^ (clear << shl);
                    q[8] &= inv ^ (clear >> shr);
                    u32 base = plane0 * color2;
                    u32 pix = base + ((page[0] & mesGlyphRowMaskHi[bit2]) >> (bit2 + 16));
                    dstCol[idx] += pix << shl;
                    q[8] += pix >> shr;
                    bit2++;
                } while (bit2 < 4);

                page += 1;
            } while (row2 < yEnd);
        }
    }
}

// MATCHED 100%
void mesGlyphBlitRow(u32 * dst, s32 x, u16 y, u16 stride, u32 * page, u8 glyph, u8 color) {
    u32 * p;
    s16 bit;
    u32 shr;
    u32 shl;
    s16 colsLeft;
    s16 row;
    u32 * dstCol;
    u16 yEnd;
    u32 color1;
    u32 rowOff2;
    s16 row0;
    u32 rowOff;
    u16 widthPx;
    s16 row2;
    u32 inv;
    u32 * p2;
    s16 bit2;
    u32 color2;
    u32 inv2;
    MesGlyphHeader header;

    header.raw = page[0];

    u32 cellW = (u16)((header.raw & 0xf0) >> 4);
    shl = (u16)((x & 7) << 2);
    u16 cellH = header.raw & 15;
    page = page + 1 + ((header.raw & 0xff000000) >> 24);
    u32 cellWords = glyph * cellW;
    u32 skip = cellH * cellWords;
    shr = (u16)(32 - shl);
    page += skip;
    dstCol = dst + (x >> 3);
    yEnd = y + cellH * 4;
    colsLeft = (u16)(cellW * 4) & 0xfff8;
    widthPx = cellW * 4;

    if (colsLeft > 0) {
        color1 = color;
        row0 = (s16)y;

        do {
            row = row0;

            if (row < yEnd) {
                rowOff = row * stride;
                inv = 0xffffffff;

                do {
                    p = dstCol + rowOff;
                    bit = 0;

                    do {
                        u32 mask = mesGlyphRowMask[bit];
                        u32 plane0 = (page[0] & mask) >> bit;
                        u32 clear = plane0 * 16 - plane0;
                        u32 base = plane0 * color1;

                        p[0] &= inv ^ (clear << shl);
                        p[1] &= inv ^ (clear >> shr);
                        u32 pix = (page[1] & mask) >> bit;
                        pix += base;
                        p[0] += pix << shl;
                        p[1] += pix >> shr;

                        p += stride;
                        bit++;
                    } while (bit < 4);

                    page += 2;
                    rowOff += stride * 4;
                    row += 4;
                } while (row < yEnd);
            }

            colsLeft -= 8;
            dstCol += 1;
        } while (colsLeft > 0);
    }

    if (widthPx & 4) {
        row2 = (s16)y;

        if (row2 < yEnd) {
            color2 = color;
            rowOff2 = row2 * stride;

            do {
                p2 = dstCol + rowOff2;
                bit2 = 0;
                inv2 = 0xffffffff;

                do {
                    u32 plane0 = (page[0] & mesGlyphRowMaskLo[bit2]) >> bit2;
                    u32 clear = plane0 * 16 - plane0;

                    p2[0] &= inv2 ^ (clear << shl);
                    u32 base = plane0 * color2;
                    p2[1] &= inv2 ^ (clear >> shr);
                    u32 pix = (page[0] & mesGlyphRowMaskHi[bit2]) >> (bit2 + 16);
                    pix += base;
                    p2[0] += pix << shl;
                    p2[1] += pix >> shr;

                    p2 += stride;
                    bit2++;
                } while (bit2 < 4);

                page += 1;
                row2 += 4;
                rowOff2 += stride * 4;
            } while (row2 < yEnd);
        }
    }
}

// MATCHED 100%
void mesGlyphBlitTileScaled(u32 * dst, u16 x, u16 y, s32 scale, u16 stride, u32 * page, u8 glyph, u8 color) {
    u32 * dstCol;
    s16 dx;
    u32 mask;
    u8 sh;
    u32 inv;
    s16 colsLeft;
    s16 row;
    u32 header;
    s16 bit;
    u16 idx2;
    s16 row0;
    s32 wide2;
    s32 wide;
    u32 color1;
    u32 shr;
    u32 tall;
    s32 dxEnd;
    s32 vstep;
    u32 stride32;
    u32 shl;
    u32 widthPx;
    s16 bit2;
    u16 idxb;
    u16 yEnd;
    u32 color2;
    u32 colsLeft0;
    u32 tall2;
    const u32 * masks;
    u32 * dstCol2;
    s16 row2;

    shl = (u16)((x & 7) << 2);
    header = page[0];
    stride32 = stride;
    u16 cellH = header & 15;
    u32 cellW = (u16)((header & 0xf0) >> 4);
    u32 cellWords = glyph * cellW;
    shr = (u16)(32 - shl);
    u32 skip = cellH * cellWords;
    page = page + 1 + ((header & 0xff000000) >> 24) + skip;
    dst += (x & 0xfff8) + (y & 0xfff8) * stride32;
    y &= 7;
    vstep = scale >> 1;
    dstCol = dst;
    yEnd = y + ((cellH * 4) << vstep);
    colsLeft0 = (u16)(cellW * 4) & 0xfff8;
    colsLeft = colsLeft0;
    widthPx = (u16)(cellW * 4);

    if (colsLeft > 0) {
        color1 = color;
        row0 = (s16)y;
        wide = scale & 1;
        tall = scale & 2;
        dxEnd = wide << 3;

        do {
            row = row0;

            if (row < yEnd) {
                inv = 0xffffffff;

                do {
                    masks = wide ? mesGlyphRowMaskLo : mesGlyphRowMask;
                    bit = 0;

                    do {
                        u16 idx = (row & 0xfff8) * stride32 + (row & 7);
                        row++;
                        mask = masks[bit];
                        sh = bit;
                        dx = 0;

                        if (dx <= dxEnd) {
                            idx2 = (row & 0xfff8) * stride32 + (row & 7);

                            do {
                                u32 plane0 = (page[0] & mask) >> sh;
                                u32 pix = plane0 * color1 + ((page[1] & mask) >> sh);

                                if (wide) {
                                    plane0 = (plane0 & 15) + ((plane0 & 0xf0) << 4) + ((plane0 & 0xf00) << 8) + ((plane0 & 0xf000) << 12);
                                    plane0 |= plane0 << 4;

                                    pix = (pix & 15) + ((pix & 0xf0) << 4) + ((pix & 0xf00) << 8) + ((pix & 0xf000) << 12);
                                    pix |= pix << 4;
                                }

                                u32 clear = plane0 * 16 - plane0;
                                u32 keepLo = inv ^ (clear << shl);
                                u32 keepHi = inv ^ (clear >> shr);
                                u32 * r = dstCol + idx + dx;

                                (dstCol + idx)[dx] = (dstCol + idx)[dx] & keepLo;
                                r[8] &= keepHi;
                                (dstCol + idx)[dx] = (dstCol + idx)[dx] + (pix << shl);
                                r[8] += pix >> shr;

                                if (tall) {
                                    u32 k = dx + idx2;
                                    u32 * r2 = dstCol + k;

                                    dstCol[k] &= keepLo;
                                    r2[8] &= keepHi;
                                    dstCol[k] += pix << shl;
                                    r2[8] += pix >> shr;
                                }

                                mask = mask << 16;
                                sh += 16;
                                dx += 8;
                            } while (dx <= dxEnd);
                        }

                        row += vstep;
                        bit++;
                    } while (bit < 4);

                    page += 2;
                } while (row < yEnd);
            }

            colsLeft -= 8;
            dstCol += 8;
        } while (colsLeft > 0);
    }

    if (widthPx & 4) {
        wide2 = scale & 1;
        dstCol2 = dst + (colsLeft0 << wide2);
        row2 = (s16)y;

        if (row2 < yEnd) {
            color2 = color;
            tall2 = scale & 2;

            do {
                bit2 = 0;
                u32 inv2 = 0xffffffff;

                do {
                    idxb = (row2 & 0xfff8) * stride32 + (row2 & 7);
                    row2++;
                    u32 v = page[0];
                    u32 plane0 = (v & mesGlyphRowMaskLo[bit2]) >> bit2;
                    u32 pix = plane0 * color2 + ((v & mesGlyphRowMaskHi[bit2]) >> (bit2 + 16));

                    if (wide2) {
                        plane0 = (plane0 & 15) + ((plane0 & 0xf0) << 4) + ((plane0 & 0xf00) << 8) + ((plane0 & 0xf000) << 12);
                        plane0 |= plane0 << 4;

                        pix = (pix & 15) + ((pix & 0xf0) << 4) + ((pix & 0xf00) << 8) + ((pix & 0xf000) << 12);
                        pix |= pix << 4;
                    }

                    u32 clear = plane0 * 16 - plane0;
                    u32 keepLo = inv2 ^ (clear << shl);
                    u32 keepHi = inv2 ^ (clear >> shr);
                    u32 * r = dstCol2 + idxb;

                    dstCol2[idxb] &= keepLo;
                    r[8] &= keepHi;
                    dstCol2[idxb] += pix << shl;
                    r[8] += pix >> shr;

                    if (tall2) {
                        u16 idx3 = (row2 & 0xfff8) * stride32 + (row2 & 7);
                        row2++;
                        u32 * r3 = dstCol2 + idx3;

                        dstCol2[idx3] &= keepLo;
                        r3[8] &= keepHi;
                        dstCol2[idx3] += pix << shl;
                        r3[8] += pix >> shr;
                    }

                    bit2++;
                } while (bit2 < 4);

                page += 1;
            } while (row2 < yEnd);
        }
    }
}

// MATCHED 100%
void mesGlyphBlitRowScaled(u32 * dst, s32 x, u16 y, s32 scale, u16 stride, u32 * page, u8 glyph, u8 color) {
    u32 header;
    u32 * p;
    s16 dx;
    u32 mask;
    u32 shl;
    u32 stride32;
    u8 sh;
    s32 colsLeft;
    s16 row;
    u32 plane0;
    u32 shr;
    const u32 * masks;
    u32 * dstCol;
    s16 bit;
    u32 rowOff2;
    u32 rowStride16;
    u32 inv2;
    u32 inv;
    s32 wide;
    u32 color1;
    u32 tall;
    u32 rowOff;
    u32 stride2;
    s32 widthPx;
    s32 heightPx;
    u32 * p2;
    u32 rowStride;
    s16 bit2;
    u32 * dstCol2;
    u32 color2;
    u32 plane0b;
    s16 row2;
    u32 tall2;
    s32 wide2;

    stride32 = stride;
    header = page[0];
    u32 off = y * stride32;
    u32 cellW = (u16)((header & 0xf0) >> 4);
    shl = (u16)((x & 7) << 2);
    rowStride = stride32 << (scale >> 1);
    u32 cellWords = glyph * cellW;
    shr = (u16)(32 - shl);
    u16 cellH = header & 15;
    u32 skip = cellH * cellWords;
    rowStride16 = (u16)rowStride;
    page = page + 1 + ((header & 0xff000000) >> 24) + skip;
    u32 off2 = off + (x >> 3);
    dst += off2;
    dstCol = dst;
    heightPx = (u16)(cellH * 4);
    widthPx = (u16)(cellW * 4);
    colsLeft = (s16)(widthPx & 0xfff8);

    if (colsLeft > 0) {
        color1 = color;
        wide = scale & 1;
        tall = scale & 2;

        do {
            row = 0;

            if (row < heightPx) {
                rowOff = row;
                inv = 0xffffffff;

                do {
                    p = dstCol + rowOff;
                    masks = wide ? mesGlyphRowMaskLo : mesGlyphRowMask;
                    bit = 0;

                    do {
                        mask = masks[bit];
                        sh = bit;
                        dx = 0;

                        if (dx <= wide) {
                            u32 * q = p + stride32;

                            do {
                                plane0 = (page[0] & mask) >> sh;
                                u32 pix = plane0 * color1 + ((page[1] & mask) >> sh);

                                if (wide) {
                                    plane0 = (plane0 & 15) + ((plane0 & 0xf0) << 4) + ((plane0 & 0xf00) << 8) + ((plane0 & 0xf000) << 12);
                                    plane0 |= plane0 << 4;

                                    pix = (pix & 15) + ((pix & 0xf0) << 4) + ((pix & 0xf00) << 8) + ((pix & 0xf000) << 12);
                                    pix |= pix << 4;
                                }

                                u32 clear = plane0 * 16 - plane0;
                                u32 keepLo = inv ^ (clear << shl);
                                u32 keepHi = inv ^ (clear >> shr);
                                u32 * r = p + dx;

                                p[dx] &= keepLo;
                                r[1] &= keepHi;
                                p[dx] += pix << shl;
                                r[1] += pix >> shr;

                                if (tall) {
                                    u32 * r2 = q + dx;

                                    q[dx] &= keepLo;
                                    r2[1] &= keepHi;
                                    q[dx] += pix << shl;
                                    r2[1] += pix >> shr;
                                }

                                mask = mask << 16;
                                sh += 16;
                                dx++;
                            } while (dx <= wide);
                        }

                        p += rowStride;
                        bit++;
                    } while (bit < 4);

                    page += 2;
                    rowOff += rowStride16 * 4;
                    row += 4;
                } while (row < heightPx);
            }

            dstCol += 1;
            colsLeft = (s16)(colsLeft - 8);
        } while (colsLeft > 0);
    }

    if (widthPx & 4) {
        wide2 = scale & 1;
        dstCol2 = dst + ((widthPx >> 3) << wide2);
        row2 = 0;

        if (row2 < heightPx) {
            rowOff2 = row2;
            tall2 = scale & 2;
            color2 = color;
            stride2 = stride;

            do {
                inv2 = 0xffffffff;
                bit2 = 0;
                p2 = dstCol2 + rowOff2;

                do {
                    u32 v = page[0];
                    plane0b = (v & mesGlyphRowMaskLo[bit2]) >> bit2;
                    u32 pix = plane0b * color2 + ((v & mesGlyphRowMaskHi[bit2]) >> (bit2 + 16));

                    if (wide2) {
                        plane0b = (plane0b & 15) + ((plane0b & 0xf0) << 4) + ((plane0b & 0xf00) << 8) + ((plane0b & 0xf000) << 12);
                        plane0b |= plane0b << 4;

                        pix = (pix & 15) + ((pix & 0xf0) << 4) + ((pix & 0xf00) << 8) + ((pix & 0xf000) << 12);
                        pix |= pix << 4;
                    }

                    u32 clear = plane0b * 16 - plane0b;
                    u32 keepLo = inv2 ^ (clear << shl);
                    u32 keepHi = inv2 ^ (clear >> shr);

                    p2[0] &= keepLo;
                    p2[1] &= keepHi;
                    p2[0] += pix << shl;
                    p2[1] += pix >> shr;

                    if (tall2) {
                        u32 * r2 = p2 + stride2;

                        p2[stride2] &= keepLo;
                        r2[1] &= keepHi;
                        p2[stride2] += pix << shl;
                        r2[1] += pix >> shr;
                    }

                    p2 += rowStride;
                    bit2++;
                } while (bit2 < 4);

                row2 += 4;
                page += 1;
                rowOff2 += rowStride16 * 4;
            } while (row2 < heightPx);
        }
    }
}

#pragma opt_common_subs reset
#pragma optimization_level reset
