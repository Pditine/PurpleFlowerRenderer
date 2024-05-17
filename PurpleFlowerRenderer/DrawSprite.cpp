#include "DrawSprite.h"
#include "easyx.h"


void drawAlpha(IMAGE* picture, int picture_x, int picture_y)
{
    // 变量初始化
    DWORD* dst = GetImageBuffer();
    DWORD* draw = GetImageBuffer();
    DWORD* src = GetImageBuffer(picture);
    int picture_width = picture->getwidth();
    int picture_height = picture->getheight();
    int graphWidth = getwidth();
    int graphHeight = getheight();
    int dstX = 0;

    // 实现透明贴图
    for (int iy = 0; iy < picture_height; iy++)
    {
        for (int ix = 0; ix < picture_width; ix++)
        {
            int srcX = ix + iy * picture_width;
            int sa = ((src[srcX] & 0xff000000) >> 24);
            int sr = ((src[srcX] & 0xff0000) >> 16);
            int sg = ((src[srcX] & 0xff00) >> 8);
            int sb = src[srcX] & 0xff;

            int targetX = ix + picture_x;
            int targetY = iy + picture_y;

            if (targetX >= 0 && targetX < graphWidth && targetY >= 0 && targetY < graphHeight)
            {
                dstX = targetX + targetY * graphWidth;
                int dr = ((dst[dstX] & 0xff0000) >> 16);
                int dg = ((dst[dstX] & 0xff00) >> 8);
                int db = dst[dstX] & 0xff;

                draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)
                    | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)
                    | (sb * sa / 255 + db * (255 - sa) / 255);
            }
        }
    }
}