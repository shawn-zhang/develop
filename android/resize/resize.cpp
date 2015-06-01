/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cutils/memory.h>

#include <utils/Log.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>

#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <android/native_window.h>

#include <gui/DisplayEventReceiver.h>
#include <utils/Looper.h>

using namespace android;

//namespace android {

static sp<Surface> surface;

int receiver(int fd, int events, void* data)
{
    DisplayEventReceiver* q = (DisplayEventReceiver*)data;

    ssize_t n;
    DisplayEventReceiver::Event buffer[1];

    static nsecs_t oldTimeStamp = 0;

    while ((n = q->getEvents(buffer, 1)) > 0) {
        for (int i=0 ; i<n ; i++) {
            if (buffer[i].header.type == DisplayEventReceiver::DISPLAY_EVENT_VSYNC) {
                ANativeWindow_Buffer outBuffer;
                surface->lock(&outBuffer, NULL);
#if 0
                ssize_t bpr = outBuffer.stride * bytesPerPixel(outBuffer.format);
                android_memset16((uint16_t*)outBuffer.bits, random() & 0xFFFF, bpr*outBuffer.height);
#else
                char *vaddr = (char *)outBuffer.bits;
                ssize_t bpr = outBuffer.width * outBuffer.height * bytesPerPixel(outBuffer.format);
                for (int i = 0; i < bpr; i += 4) {
                    vaddr[0] =  0xFF;
                    vaddr[1] =  0x00;
                    vaddr[2] =  0x00;
                    vaddr[3] =  0x00;
                    vaddr = vaddr + 4;
                }
#endif
                surface->unlockAndPost();
            }
        }
    }
    if (n<0) {
        printf("error reading events (%s)\n", strerror(-n));
    }
    return 1;
}

int main(int argc, char** argv)
{
    // set up the thread-pool
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();

    // create a client to surfaceflinger
    sp<SurfaceComposerClient> client = new SurfaceComposerClient();
    
    sp<SurfaceControl> surfaceControl = client->createSurface(String8("resize"),
            300, 240, HAL_PIXEL_FORMAT_RGBA_8888, 0);

    surface = surfaceControl->getSurface();

    SurfaceComposerClient::openGlobalTransaction();
    surfaceControl->setLayer(100000);
    //surfaceControl->setAlpha(0.5);
    surfaceControl->setPosition(400,200);
    SurfaceComposerClient::closeGlobalTransaction();

    DisplayEventReceiver myDisplayEvent;
    sp<Looper> loop = new Looper(false);
    loop->addFd(myDisplayEvent.getFd(), 0, ALOOPER_EVENT_INPUT, receiver, &myDisplayEvent);
    myDisplayEvent.setVsyncRate(10);
    
    do {
        int32_t ret = loop->pollOnce(-1);
        switch (ret) {
            case ALOOPER_POLL_WAKE:
                break;
            case ALOOPER_POLL_CALLBACK:
                break;
            case ALOOPER_POLL_TIMEOUT:
                printf("ALOOPER_POLL_TIMEOUT\n");
                break;
            case ALOOPER_POLL_ERROR:
                printf("ALOOPER_POLL_TIMEOUT\n");
                break;
            default:
                printf("ugh? poll returned %d\n", ret);
                break;
        }
    } while (1);

    IPCThreadState::self()->joinThreadPool();
    
    return 0;
}
