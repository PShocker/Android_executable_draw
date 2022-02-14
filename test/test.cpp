#include <cutils/memory.h>

#include <utils/Log.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <android/native_window.h>

#include <SkBitmap.h>
#include <SkCanvas.h>
#include <SkColor.h>
#include <SkPaint.h>
#include <SkRect.h>

#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>

#include <ui/DisplayInfo.h>
#include <hardware/hwcomposer_defs.h>

using namespace android;

// static void flush_buffer(sp<Surface> s, ANativeWindow_Buffer outBuffer)
// {
//   uint8_t *img = reinterpret_cast<uint8_t *>(outBuffer.bits);
//   for (uint32_t y = 0; y < outBuffer.height; y++)
//   {
//     for (uint32_t x = 0; x < outBuffer.width; x++)
//     {
//       uint8_t *pixel = img + (4 * (y * outBuffer.stride + x));
//       pixel[0] = 0;
//       pixel[1] = 0;
//       pixel[2] = 0;
//       pixel[3] = 0;
//       printf("img:%x pixel:%x \n", img, pixel);
//     }
//   }
// }

static void clear_buffer(sp<Surface> s, ANativeWindow_Buffer outBuffer)
{
  uint8_t *img = reinterpret_cast<uint8_t *>(outBuffer.bits);
  // memset(img,0,0x383FFC);
  memset(img, 0, 4 * ((outBuffer.height - 1) * outBuffer.stride + outBuffer.width - 1));
}

int main()
{
  // set up the thread-pool
  sp<ProcessState> proc(ProcessState::self());
  ProcessState::self()->startThreadPool();

  // create a client to surfaceflinger
  sp<SurfaceComposerClient> client = new SurfaceComposerClient();

  DisplayInfo display;

  //获取屏幕的宽高等信息

  client->getDisplayInfo(client->getBuiltInDisplay(HWC_DISPLAY_PRIMARY), &display);

  sp<SurfaceControl> surfaceControl = client->createSurface(String8("test"),
                                                            display.w, display.h, PIXEL_FORMAT_RGBA_8888, ISurfaceComposerClient::eHidden);

  sp<Surface> surface = surfaceControl->getSurface();

  SurfaceComposerClient::openGlobalTransaction();
  surfaceControl->setLayer(100000);
  surfaceControl->setPosition(0, 0);
  surfaceControl->show();
  SurfaceComposerClient::closeGlobalTransaction();

  int i = 1;
  while (true)
  {
    ANativeWindow_Buffer outBuffer;
    surface->lock(&outBuffer, NULL);

    clear_buffer(surface, outBuffer);

    SkBitmap surfaceBitmap;
    ssize_t bpr = outBuffer.stride * bytesPerPixel(outBuffer.format);

    SkImageInfo info = SkImageInfo::Make(display.w, display.h, kN32_SkColorType, kPremul_SkAlphaType);
    surfaceBitmap.installPixels(info, outBuffer.bits, bpr);

    SkCanvas surfaceCanvas(surfaceBitmap);
    SkPaint paint;
    SkRect rect{i, i, display.w - i, display.h - i};

    // surfaceCanvas.drawColor(0xFFFF0000);
    paint.setColor(SK_ColorRED);
    paint.setStrokeWidth(2);
    paint.setStyle(SkPaint::kStroke_Style);
    surfaceCanvas.drawRect(rect, paint);

    surface->unlockAndPost();
    if (i == display.w)
    {
      i = 0;
    }

    i++;
  }

  IPCThreadState::self()->joinThreadPool();

  return 0;
}