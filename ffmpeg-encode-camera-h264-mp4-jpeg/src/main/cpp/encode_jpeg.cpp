//
// Created by KE TU on 2020-08-10.
//

#include <libyuv.h>
#include "encode_jpeg.h"
#include "logger.h"


/**
 * 编码 开关
 * @return
 */
bool JPEGEncoder::isTransform() {
    return transform;
}

/**
 * 构造函数
 *
 * @param jpegPath
 * @param width
 * @param height
 */
JPEGEncoder::JPEGEncoder(const char *jpegPath, int width, int height) {
    LOGI("JPEGEncoder");
    LOGI("jpegPath = %s, width = %d, height = %d", jpegPath, width, height);

    this->width = width;
    this->height = height;
    this->jpegPath = jpegPath;

    //1.注册所有组件
    av_register_all();
    //2. 初始化输出码流的AVFormatContext
    avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, this->jpegPath);

    //3.打开待输出的视频文件
    if (avio_open(&pFormatCtx->pb, this->jpegPath, AVIO_FLAG_READ_WRITE)) {
        LOGE("Could not open output file");
        return;
    }

    //4.初始化视频码流
    pStream = avformat_new_stream(pFormatCtx, NULL);
    if (pStream == NULL) {
        LOGE("Could not allocating output stream");
        return;
    }
    //5.寻找编码器并打开编码器
    pCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!pCodec) {
        LOGE("Could not find encoder");
        return;
    }

    //6.分配编码器并设置参数
    pCodecCtx = avcodec_alloc_context3(pCodec);
    pCodecCtx->codec_id = pCodec->id;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    // 注意宽高
    pCodecCtx->width  = height;
    pCodecCtx->height = width;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;
    pCodecCtx->bit_rate = 400000;
    pCodecCtx->gop_size = 12;


    //将AVCodecContext的成员复制到AVCodecParameters结构体
    avcodec_parameters_from_context(pStream->codecpar, pCodecCtx);

    //7.打开编码器
    int ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0) {
        LOGE("Could not open encoder");
        return;
    }

    // 设置 开始转换
    this->transform = true;

    LOGI("开始转换: transform = true");
}

/**
 * 析构函数
 */
JPEGEncoder::~JPEGEncoder() {
    LOGI("~JPEGEncoder");
    // 标记转换结束
    this->transform = false;

    avcodec_close(pCodecCtx);
    av_free(pFrame);
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
}

/**
 * 编码 jpeg-------实际上 仅仅是编码一帧 nv21图像
 * @param nv21Buffer
 * @return
 */
int JPEGEncoder::encodeJPEG(unsigned char *nv21Buffer) {
    LOGI("encodeJPEG");

    // 初始化图像帧
    pFrame = av_frame_alloc();
    pFrame->width = pCodecCtx->width;
    pFrame->height = pCodecCtx->height;
    pFrame->format = pCodecCtx->pix_fmt;
    int bufferSize = av_image_get_buffer_size(pCodecCtx->pix_fmt,
            pCodecCtx->width,
            pCodecCtx->height, 1);
    out_buffer = (uint8_t *) av_malloc(bufferSize);
    // 设置 图像帧 参数
    av_image_fill_arrays(pFrame->data,
            pFrame->linesize,
            out_buffer,
            pCodecCtx->pix_fmt,
            pCodecCtx->width,
            pCodecCtx->height, 1);

    //8.写文件头
    avformat_write_header(pFormatCtx, NULL);

    //创建已编码帧
    av_new_packet(&avPacket, bufferSize * 3);

    // y,u,v分量起始地址
    uint8_t *i420_y = out_buffer;
    uint8_t *i420_u = out_buffer + width * height;
    uint8_t *i420_v = out_buffer + width * height * 5 / 4;

    //NV21转I420
    // 前置摄像头，旋转270 libyuv::kRotate270,
    // 后置摄像头，旋转90
    libyuv::ConvertToI420(nv21Buffer,
            width * height,
            i420_y, height,
            i420_u, height / 2,
            i420_v, height / 2,
            0, 0,
            width, height,
            width, height,
            libyuv::kRotate90,
            libyuv::FOURCC_NV21);


    // 这里可以再作一次镜像翻转,不然你看到的前置相机的图片是反向的

    pFrame->data[0] = i420_y;
    pFrame->data[1] = i420_u;
    pFrame->data[2] = i420_v;

    if (avcodec_send_frame(pCodecCtx, pFrame) < 0) {
        return -1;
    }

    while (avcodec_receive_packet(pCodecCtx, &avPacket) == 0) {
        avPacket.stream_index = pStream->index;
        av_interleaved_write_frame(pFormatCtx, &avPacket);
        av_packet_unref(&avPacket);
    }

    //9.写文件尾
    av_write_trailer(pFormatCtx);

    LOGI("encodeJPEG: success ");
    return 0;
}

