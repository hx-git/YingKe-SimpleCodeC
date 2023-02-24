//
// Created by KE TU on 2020-07-19.
//

#include "jni.h"
#include <ctime>
#ifndef YINGKE_SIMPLECODEC_NATIVE_YUV2RGB_H
#define YINGKE_SIMPLECODEC_NATIVE_YUV2RGB_H
#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT void JNICALL yuv2rgb(JNIEnv *env, jclass thiz, jstring yuvPath, jint type, jint width, jint height, jobject surface) ;
JNIEXPORT void JNICALL rgb2yuv(JNIEnv *env,
                               jclass thiz,
                               jstring rgb24Path,
                               jstring yuv420pPath,
                               jint width,
                               jint height);


inline int calc_time(clock_t start) {
    return (int) (1000 * (clock() - start) / CLOCKS_PER_SEC);
}

#ifdef __cplusplus
}
#endif
#endif //YINGKE_SIMPLECODEC_NATIVE_YUV2RGB_H
