//
// Created by KE TU on 2020-08-10.
//

#ifndef YINGKE_SIMPLECODEC_ENCODE_MP4_H
#define YINGKE_SIMPLECODEC_ENCODE_MP4_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>

#ifdef __cplusplus
}
#endif

class Mp4Encoder {

protected:
    /**
     * 编码开关
     */
    bool transform = false;

    const char *mp4Path;
    int width;
    int height;

    AVFormatContext *avFormatContext = NULL;
    AVCodecContext *avCodecContext = NULL;
    AVCodec *avCodec = NULL;
    AVStream *avStream = NULL;
    AVPacket avPacket;
    AVFrame *avFrame = NULL;

    uint8_t *pFrameBuffer = NULL;

    // AVFrame PTS
    int index = 0;

public:

    /**
     * 编码开关
     * @return
     */
    bool isTransform();

    /**
     * 初始化
     * @param mp4Path
     * @param width
     * @param height
     */
    void initEncoder(const char* mp4Path, int width, int height);

    /**
     * 编码开始
     */
    void encodeStart();

    /**
     * 编码 nv21 buffer
     * @param nv21Buffer
     */
    void encodeBuffer(unsigned char *nv21Buffer);

    /**
     * 编码 一帧图像
     * @param pCodecCtx
     * @param pAvFrame
     * @param pAvPacket
     * @return
     */
    int encodeFrame(AVCodecContext *pCodecCtx, AVFrame *pAvFrame, AVPacket *pAvPacket);

    /**
     * 编码开始
     */
    void encodeStop();
};


#endif //YINGKE_SIMPLECODEC_ENCODE_MP4_H
