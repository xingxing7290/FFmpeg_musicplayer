// #include <libavformat/avformat.h>

// int main() {
//     AVFormatContext *pFormatCtx = NULL;
//     int i, videoStream, audioStream;
//     AVCodecContext *pCodecCtx = NULL;

//     // Register all formats and codecs
//     av_register_all();

//     // Open video file
//     if(avformat_open_input(&pFormatCtx, "your_video_file", NULL, NULL)!=0)
//         return -1; // Couldn't open file

//     // Retrieve stream information
//     if(avformat_find_stream_info(pFormatCtx, NULL)<0)
//         return -1; // Couldn't find stream information

//     // Find the first video stream
//     videoStream=-1;
//     audioStream=-1;
//     for(i=0; i<pFormatCtx->nb_streams; i++) {
//         if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO && videoStream < 0) {
//             videoStream=i;
//         }
//         if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO && audioStream < 0) {
//             audioStream=i;
//         }
//     }

//     if(videoStream==-1)
//         return -1; // Didn't find a video stream
//     if(audioStream==-1)
//         return -1; // Didn't find an audio stream

//     // Get a pointer to the codec context for the video stream
//     pCodecCtx=pFormatCtx->streams[videoStream]->codec;

//     // Find the decoder for the video stream
//     AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
//     if(pCodec==NULL) {
//         fprintf(stderr, "Unsupported codec!\n");
//         return -1; // Codec not found
//     }

//     // Open codec
//     if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
//         return -1; // Could not open codec

//     // Allocate video frame
//     AVFrame *pFrame = av_frame_alloc();

//     // Read frames
//     AVPacket packet;
//     while(av_read_frame(pFormatCtx, &packet)>=0) {
//         // Is this a packet from the video stream?
//         if(packet.stream_index==videoStream) {
//             // Decode video frame
//             avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
//         }
//         if(packet.stream_index==audioStream) {
//             // Decode audio frame
//             avcodec_decode_audio4(pCodecCtx, pFrame, &frameFinished, &packet);
//         }

//         // Free the packet that was allocated by av_read_frame
//         av_free_packet(&packet);
//     }

//     // Free the video frame
//     av_frame_free(&pFrame);

//     // Close the codec
//     avcodec_close(pCodecCtx);

//     // Close the video file
//     avformat_close_input(&pFormatCtx);

//     return 0;
// }

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/avstring.h>
#include <libavutil/pixfmt.h>
#include <libavutil/log.h>
//#include "SDL2/SDL.h"
#include "SDL.h"
#include "SDL_thread.h"
#include <stdio.h>
#include <math.h>



int g_quit=0;
#undef main
int main(int argc, char *argv[])
{   

    printf("open file %s \n", argv[1]);
    int ret;
    //AVFormarContext(类似C++类里的成员变量)
    //AVInputFormat(对应的C++类里的成员方法函数)
    AVFormatContext *pFormatCtx = NULL;

    // 分配上下文
    pFormatCtx = avformat_alloc_context();
    ret = avformat_open_input(&pFormatCtx, argv[1], NULL, NULL);
    if (ret < 0)
    {
        printf("open failed \n ");
        return -1;
    }
    //读取音视频包，获取解码器相关的信息
    ret = avformat_find_stream_info(pFormatCtx, NULL);
    if (ret < 0)
    {
        printf("find stream failed ");
        return -1;
    }


    //分离音视频流
    int video_index=  av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    int audio_index=  av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (video_index < 0 || audio_index < 0)
    {
        printf("fvideo_index ");
        return -1;
    }
    printf("video_index %d \n", video_index);
    printf("audio_index %d \n", audio_index);

    //查找视频解码器

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(NULL);
    if(!pCodecCtx)
    {
        printf("avcodec_alloc_context3 failed \n");
        return -1;
    }

    //查找音频解码器
    AVCodecContext *pAudioCodecCtx = avcodec_alloc_context3(NULL);//
    if(!pAudioCodecCtx)
    {
        printf("avcodec_alloc_context3 failed \n");
        return -1;
    }

    //拷贝解码器参数
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[video_index]->codecpar);
    if (ret < 0)
    {
        printf("avcodec_parameters_to_context failed \n");
        return -1;
    }
    ret= avcodec_parameters_to_context(pAudioCodecCtx, pFormatCtx->streams[audio_index]->codecpar);
    if (ret < 0)
    {
        printf("avcodec_parameters_to_context failed \n");
        return -1;
    }


    //查找视频解码器
    const AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (!pCodec)
    {
        printf("avcodec_find_decoder failed \n");
        return -1;
    }
    //查找音频解码器
    const AVCodec *pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
    if (!pAudioCodec)
    {
        printf("avcodec_find_decoder failed \n");
        return -1;
    }


    //打开视频解码器，将解码器和解码器上下文关联
    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if (ret < 0)
    {
        printf("avcodec_open2 failed \n");
        return -1;
    }


    //打开音频解码器，将解码器和解码器上下文关联
    ret = avcodec_open2(pAudioCodecCtx, pAudioCodec, NULL);
    if (ret < 0)
    {
        printf("avcodec_open2 failed \n");
        return -1;
    }
    printf("-----------------------file open---------------------\n");
    av_dump_format(pFormatCtx, 0, argv[1], 0);
    printf("-----------------------file open---------------------\n");

    AVPacket *packet=av_packet_alloc();
    av_init_packet(packet);

    AVFrame *pFrame = av_frame_alloc();
    AVFrame *pAudioFrame = av_frame_alloc();


    while(g_quit!=1)
    {
        ret=av_read_frame(pFormatCtx, packet);
        if(ret<0)
        {
            printf("av_read_frame end of  \n");
            break;
        }

        if(packet->stream_index==video_index)//分离音视频包
        {
            printf("video packet \n");
            //发送原始包
            ret=avcodec_send_packet(pCodecCtx, packet);
            if(ret!=0)
            {
                printf("avcodec_send_packet failed  ret :%d\n",ret);
                return -1;
            }
            //读取解码后的包
            ret=avcodec_receive_frame(pCodecCtx, pFrame);
            if(ret!=0)
            {
                printf("avcodec_receive_frame failed  ret :%d\n",ret);
                return -1;
            }
            //
        }
        else if(packet->stream_index==audio_index)
        {
            printf("audio packet \n");
            ret=avcodec_send_packet(pAudioCodecCtx, packet);
            ret=avcodec_receive_frame(pAudioCodecCtx, pAudioFrame);


        }else
        {
            printf("other packet \n");
        }
        av_packet_unref(packet);//释放packet内存

    }

    printf("main finash\n");
    return 0;

}
