// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clMesWin.h"
#include "main/mesWinUtil.h"

extern "C" {

#include "main/uiUtil.h"

// --- Struct Definitions --- //

#define MESWIN_MIN_X 0
#define MESWIN_MIN_Y 1
#define MESWIN_MAX_X 2
#define MESWIN_MAX_Y 3

// --- Extern Functions --- //

extern void func_ov000_02063ba8(clMesWin *, MesWinParams *, s16);

// --- Variable Definitions --- //

s16 mesWinBounds[4] = {8, 8, 248, 184};  // 0x02052640

// --- Function Declarations --- //

void setMesWinMaxX(u16);  // 0x02014fc0
void setMesWinMaxY(u16);  // 0x02014fd0
s16 getMesWinMaxX(void);  // 0x02014fe0
s16 getMesWinMaxY(void);  // 0x02014ff0
u8 * getTextChunk(u32 *, u32);  // 0x02015000
u8 * getTextLines(u32 *, u32);  // 0x0201500c
u8 * getTextCols(u32 *, u32);  // 0x02015018
void buildMesWinParams(MesWinParams *, clMesWin *, u32, u32, s16, s16, s16, s16, s16, s16, s16, s32, s32, u8, u32, s16, UnkStruct_0200b8bc_10 **, u32 *, u32, s32 *, s32 *, s16);  // 0x02015028
u32 getTextLineCount(u32 *, u32);  // 0x020152c4
u32 getTextColCount(u32 *, u32);  // 0x020152d8
void calcMesWinPlacement(clMesWin *, MesWinRequest *, s16 *, s16 *, MesWinPlacement *);  // 0x020152ec
void openMesWin(clMesWin *, u32, u32, s16, s16, s16, s16, s16, s16, s16, s16, s16, u8, u32, UnkStruct_0200b8bc_10 **, u32 *, u16, s16, s32 *, s32 *, s16);  // 0x02015d04
void openMesWinEx(clMesWin *, u32, u32, s16, s16, s16, s16, s16, s16, s16, s16, s16, u8, u32, s16, u8, UnkStruct_0200b8bc_10 **, u32 *, u16, s16, s32 *, s32 *, s16);  // 0x02015dd0

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void setMesWinMaxX(u16 maxX) {
    mesWinBounds[MESWIN_MAX_X] = maxX;
}

// MATCHED 100%
void setMesWinMaxY(u16 maxY) {
    mesWinBounds[MESWIN_MAX_Y] = maxY;
}

// MATCHED 100%
s16 getMesWinMaxX() {
    return mesWinBounds[MESWIN_MAX_X];
}

// MATCHED 100%
s16 getMesWinMaxY() {
    return mesWinBounds[MESWIN_MAX_Y];
}

// MATCHED 100%
u8 * getTextChunk(u32 * text, u32 textId) {
    return RES_PTR(text, text[textId]);
}

// MATCHED 100%
u8 * getTextLines(u32 * text, u32 textId) {
    return RES_PTR(text, text[textId]);
}

// MATCHED 100%
u8 * getTextCols(u32 * text, u32 textId) {
    return RES_PTR(text, text[textId])+1;
}

// MATCHED 100%
void buildMesWinParams(MesWinParams * params, clMesWin * window, u32 screen, u32 x, s16 y, s16 style, s16 tailKind, s16 tailAlt, s16 tailUp, s16 tailFlipH, s16 tailX, s32 width, s32 height, u8 persistent, u32 voice, s16 field04_28, UnkStruct_0200b8bc_10 ** font, u32 * text, u32 textId, s32 * numbers, s32 * result, s16 indent) {
    if (width == 0 || height == 0) {
        u8 * chunk = getTextChunk(text, textId);
        u8 cols = getTextCols(text, textId)[0] + 1;
        u8 * lines = getTextLines(text, textId);

        u32 size = window->calcSize(style, lines[0]+1, cols, chunk, indent);
        if (width == 0) {
            width = (s16)size;
        }
        if (height == 0) {
            height = (s16)(size >> 16);
        }
    }

    if (tailAlt == -1) {
        tailAlt = (height > 2) ? 1 : 0;
    }
    if (tailUp == -1) {
        tailUp = (y + height * 8 >= 0x70) ? 1 : 0;
    }
    if (tailX == -1) {
        tailX = 0;
    }

    if ((u16)x == 0x8000) {
        x = (s16)((0x100 - width*8) / 2);
    }

    window->initParams(params);

    params->screen = screen;
    params->style = style;
    params->width = width;
    params->height = height;

    params->voice = voice;

    params->tailKind = tailKind;
    params->tailAlt = tailAlt;
    params->tailUp = tailUp;
    params->tailFlipH = tailFlipH;
    params->tailX = tailX;
    params->textId = textId;
    params->field04_28 = field04_28;

    params->persistent = persistent;
    params->x = x;
    params->y = y;
    params->indent = indent;
    params->fixedSize = 1;

    params->numbers = numbers;

    params->font = font;

    params->text = text;

    params->result = result;
}

// MATCHED 100%
u32 getTextLineCount(u32 * text, u32 textId) {
    return getTextLines(text, textId)[0] + 1;
}

// MATCHED 100%
u32 getTextColCount(u32 * text, u32 textId) {
    return getTextCols(text, textId)[0] + 1;
}

// MATCHED 100%
void calcMesWinPlacement(clMesWin * window, MesWinRequest * req, s16 * target, s16 * avoid, MesWinPlacement * out) {
    s16 centerX2, centerX;
    s16 targetTop;
    u8 * textData;
    s16 tailAlt;
    s16 tailXReq;
    s16 width;
    s16 height;
    s16 targetBottom;
    s16 y;
    u32 tailFlipH;
    s16 tailY0, tailY1;
    u8 indent;
    s16 targetX;
    s16 dyTop;
    s16 dyBottom;
    u32 tailLocked;
    s16 x;

    dyBottom = target[5];
    dyTop = target[3];
    width = req->width;
    targetTop = target[1] + dyTop;
    targetBottom = target[1] + dyBottom;
    s16 style = req->style;
    u32 * text = req->text;
    u16 textId = req->textId;
    s16 tailKind = req->tailKind;
    tailAlt = req->tailAlt;
    s32 tailUp = req->tailUp;
    tailXReq = req->tailX;
    height = req->height;
    targetX = target[0];
    s16 tailX = 0;
    tailFlipH = 0;

    if (width == 0 || height == 0) {
        indent = req->indent;
        textData = getTextChunk(text, textId);
        u8 cols = getTextCols(text, textId)[0] + 1;
        u8 * lines = getTextLines(text, textId);
        u32 size = window->calcSize(style, lines[0] + 1, cols, textData, indent);
        if (width == 0) {
            width = size;
        }
        if (height == 0) {
            height = size >> 16;
        }
    }

    s16 widthPx = width * 8;
    s16 heightPx = height * 8;

    if (tailKind != 0) {
        BOOL adjustable = FALSE;
        u16 kindIdx = (u16)tailKind + 0xffff;
        if (kindIdx <= 4 && ((1 << kindIdx) & 0x15)) {
            adjustable = TRUE;
        }
        tailLocked = (adjustable == FALSE) ? 1 : 0;

        s32 tailMargin = (style == 2) ? 0x18 : 0x10;
        if (tailAlt == -1) {
            tailAlt = (height > 2) ? 1 : 0;
        }
        s32 tailH = (tailAlt == 0) ? 0xc : 0x14;
        if (tailUp == -1) {
            if (target[1] >= 0x70) {
                tailUp = 0;
            } else {
                tailUp = 1;
            }
        }
        if (tailUp == 0) {
            y = targetTop - tailH - heightPx;
        } else {
            y = targetBottom + tailH - 0xc;
        }
        if (y < mesWinBounds[MESWIN_MIN_Y]) {
            y = mesWinBounds[MESWIN_MIN_Y];
        } else if (y + heightPx > mesWinBounds[MESWIN_MAX_Y]) {
            y = mesWinBounds[MESWIN_MAX_Y] - heightPx;
        }

        if (tailXReq == -1) {
            tailX = widthPx / 2;
        } else {
            tailX = tailXReq;
        }

        x = targetX - tailX;
        s16 minX = mesWinBounds[MESWIN_MIN_X];
        if (minX > x) {
            if (tailXReq == -1) {
                tailX = tailX - (minX - x);
            }
            x = minX;
            if (tailXReq == -1 && tailX < tailMargin) {
                tailX = tailMargin;
                if (tailLocked == 0) {
                    s32 tipX = minX + tailX;
                    if (tipX > targetX) {
                        s32 tipEdge = tipX - 0x10;
                        tailKind++;
                        tailLocked = 1;
                        if (tipEdge < targetX) {
                            tailX = tailX + (targetX - tipEdge);
                        }
                    }
                }
            }
        } else {
            s16 maxX = mesWinBounds[MESWIN_MAX_X];
            s32 right = x + widthPx;
            if (right > maxX) {
                if (tailXReq == -1) {
                    tailX = tailX - (maxX - right);
                }
                x = maxX - widthPx;
                if (tailXReq == -1) {
                    s32 maxTailX = widthPx - tailMargin;
                    if (tailX > maxTailX) {
                        tailX = maxTailX;
                        if (tailLocked == 0) {
                            s32 tipX = x + tailX;
                            if (tipX < targetX) {
                                s32 tipEdge = tipX + 0x10;
                                tailKind++;
                                tailLocked = 1;
                                tailFlipH = 1;
                                if (tipEdge > targetX) {
                                    tailX = tailX - (tipEdge - targetX);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (tailLocked == 0) {
            s32 overlapsY = 0;
            tailY0 = (tailUp == 0) ? y + heightPx - 8 : y - tailH;
            tailY1 = tailH + tailY0;
            s16 top = target[1] + dyTop;
            s16 bottom = target[1] + dyBottom;
            if (tailUp == 0) {
                if (tailY1 > top - 8) {
                    overlapsY = 1;
                }
            } else {
                if (tailY0 < bottom - 8) {
                    overlapsY = 1;
                }
            }
            if (overlapsY != 0) {
                s32 shiftDir = 0;
                centerX = x + widthPx / 2;
                if (target[0] > centerX) {
                    if (tailX >= tailMargin + 0x18) {
                        shiftDir--;
                    } else if (tailX <= widthPx - 0x18 - tailMargin) {
                        shiftDir = 1;
                    }
                } else {
                    if (tailX <= widthPx - 0x18 - tailMargin) {
                        shiftDir = 1;
                    } else if (tailX >= tailMargin + 0x18) {
                        shiftDir--;
                    }
                }
                if (shiftDir > 0) {
                    s16 border = window->getBorderWidth(req->style, 1);
                    if (x + tailX + 0x18 < (s16)(centerX + (widthPx / 2 - border * 16))) {
                        tailX = tailX + 0x18;
                        tailKind++;
                        tailLocked = 1;
                    }
                } else if (shiftDir < 0) {
                    s16 border = window->getBorderWidth(req->style, 1);
                    if (x + tailX - 0x18 > (s16)(centerX - (widthPx / 2 - border * 16))) {
                        tailX = tailX - 0x18;
                        tailKind++;
                        tailLocked = 1;
                        tailFlipH = 1;
                    }
                }
            }

            if (avoid != 0) {
                s16 tailLeft, tailRight;
                if (tailLocked == 0) {
                    tailLeft = (x + tailX) - 8;
                    tailRight = tailLeft + 0x10;
                } else {
                    if (tailFlipH == 1) {
                        tailLeft = (x + tailX) - 8;
                    } else {
                        tailLeft = (x + tailX) - 0x18;
                    }
                    tailRight = tailLeft + 0x20;
                }
                s16 avoidLeft = avoid[0] + avoid[2];
                s16 avoidTop = avoid[1] + avoid[3];
                s16 avoidRight = avoid[0] + avoid[4];
                s16 avoidBottom = avoid[1] + avoid[5];
                if (tailLeft <= avoidRight && tailRight >= avoidLeft && tailY0 <= avoidBottom && tailY1 >= avoidTop) {
                    s32 shiftDir = 0;
                    centerX2 = x + widthPx / 2;
                    if (tailLocked == 0) {
                        if (target[0] < avoid[0]) {
                            if (tailX >= tailMargin + 0x18) {
                                shiftDir--;
                            } else if (tailX <= widthPx - 0x18 - tailMargin) {
                                shiftDir = 1;
                            }
                        } else {
                            if (tailX <= widthPx - 0x18 - tailMargin) {
                                shiftDir = 1;
                            } else if (tailX >= tailMargin + 0x18) {
                                shiftDir--;
                            }
                        }
                        if (shiftDir > 0) {
                            s16 border = window->getBorderWidth(req->style, 1);
                            if ((x + tailX) + 0x18 < (s16)(centerX2 + (widthPx / 2 - border * 16))) {
                                tailX = tailX + 0x18;
                                tailKind++;
                            }
                        } else if (shiftDir < 0) {
                            s16 border = window->getBorderWidth(req->style, 1);
                            if ((x + tailX) - 0x18 > (s16)(centerX2 - (widthPx / 2 - border * 16))) {
                                tailX = tailX - 0x18;
                                tailKind++;
                                tailFlipH = 1;
                            }
                        }
                    } else {
                        if (tailFlipH == 1) {
                            if (target[0] >= avoid[0]) {
                                s32 newTailX = tailX + 0x30;
                                if (newTailX <= widthPx - 0x18 - tailMargin) {
                                    tailX = newTailX;
                                    tailFlipH = 0;
                                }
                            }
                        } else {
                            if (target[0] <= avoid[0]) {
                                s32 newTailX = tailX - 0x30;
                                if (newTailX >= tailMargin + 0x18) {
                                    tailX = newTailX;
                                    tailFlipH = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        if (tailUp == -1) {
            if (target[1] >= 0x70) {
                tailUp = 0;
            } else {
                tailUp = 1;
            }
        }
        if (tailUp == 0) {
            y = targetTop - heightPx;
            if (mesWinBounds[MESWIN_MIN_Y] > y) {
                y = mesWinBounds[MESWIN_MIN_Y];
            }
        } else {
            y = targetBottom - 0xc;
            if (heightPx + y > mesWinBounds[MESWIN_MAX_Y]) {
                y = mesWinBounds[MESWIN_MAX_Y] - heightPx;
            }
        }
        x = targetX - widthPx / 2;
        if (mesWinBounds[MESWIN_MIN_X] > x) {
            x = mesWinBounds[MESWIN_MIN_X];
        }
        if (x + widthPx > mesWinBounds[MESWIN_MAX_X]) {
            x = mesWinBounds[MESWIN_MAX_X] - widthPx;
        }
    }

    out->x = x;
    out->y = y;
    out->width = width;
    out->height = height;
    out->tailKind = tailKind;
    out->tailAlt = tailAlt;
    out->tailUp = tailUp;
    out->tailFlipH = tailFlipH;
    out->tailX = tailX;
}

// MATCHED 100%
void openMesWin(clMesWin * window, u32 screen, u32 x, s16 y, s16 style, s16 tailKind, s16 tailAlt, s16 tailUp, s16 tailFlipH, s16 tailX, s16 width, s16 height, u8 persistent, u32 voice, UnkStruct_0200b8bc_10 ** font, u32 * text, u16 textId, s16 slot, s32 * numbers, s32 * result, s16 indent) {
    MesWinParams params;
    buildMesWinParams(&params, window, screen, x, y, style, tailKind, tailAlt, tailUp, tailFlipH, tailX, width, height, persistent, voice, 0, font, text, textId, numbers, result, indent);
    window->open(&params, slot);
}

// MATCHED 100%
void openMesWinEx(clMesWin * window, u32 screen, u32 x, s16 y, s16 style, s16 tailKind, s16 tailAlt, s16 tailUp, s16 tailFlipH, s16 tailX, s16 width, s16 height, u8 persistent, u32 voice, s16 field24_0, u8 field24_7, UnkStruct_0200b8bc_10 ** font, u32 * text, u16 textId, s16 slot, s32 * numbers, s32 * result, s16 indent) {
    MesWinParams params;
    buildMesWinParams(&params, window, screen, x, y, style, tailKind, tailAlt, tailUp, tailFlipH, tailX, width, height, persistent, voice, 0, font, text, textId, numbers, result, indent);

    params.field24_0 = field24_0;
    params.field24_7 = field24_7;

    func_ov000_02063ba8(window, &params, slot);
}
