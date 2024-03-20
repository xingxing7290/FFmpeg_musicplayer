// #include <libavcodec/avcodec.h>
// #include <libavformat/avformat.h>
// #include <libswscale/swscale.h>
// #include <libswresample/swresample.h>
// #include <libavutil/avstring.h>
// #include <libavutil/pixfmt.h>
// #include <libavutil/log.h>
// //#include "SDL2/SDL.h"
// #include "SDL.h"
// #include "SDL_thread.h"
// #include <stdio.h>
// #include <math.h>



// int g_quit=0;
// #undef main
// int main(int argc, char *argv[])
// {   

//     printf("open file %s \n", argv[1]);
//     int ret;
//     //AVFormarContext(类似C++类里的成员变量)
//     //AVInputFormat(对应的C++类里的成员方法函数)
//     AVFormatContext *pFormatCtx = NULL;

//     // 分配上下文
//     pFormatCtx = avformat_alloc_context();
//     ret = avformat_open_input(&pFormatCtx, argv[1], NULL, NULL);
//     if (ret < 0)
//     {
//         printf("open failed \n ");
//         return -1;
//     }
//     //读取音视频包，获取解码器相关的信息
//     ret = avformat_find_stream_info(pFormatCtx, NULL);
//     if (ret < 0)
//     {
//         printf("find stream failed ");
//         return -1;
//     }


//     //分离音视频流
//     int video_index=  av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
//     int audio_index=  av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
//     if (video_index < 0 || audio_index < 0)
//     {
//         printf("fvideo_index ");
//         return -1;
//     }
//     printf("video_index %d \n", video_index);
//     printf("audio_index %d \n", audio_index);

//     //查找视频解码器

//     AVCodecContext *pCodecCtx = avcodec_alloc_context3(NULL);
//     if(!pCodecCtx)
//     {
//         printf("avcodec_alloc_context3 failed \n");
//         return -1;
//     }

//     //查找音频解码器
//     AVCodecContext *pAudioCodecCtx = avcodec_alloc_context3(NULL);//
//     if(!pAudioCodecCtx)
//     {
//         printf("avcodec_alloc_context3 failed \n");
//         return -1;
//     }

//     //拷贝解码器参数
//     ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[video_index]->codecpar);
//     if (ret < 0)
//     {
//         printf("avcodec_parameters_to_context failed \n");
//         return -1;
//     }
//     ret= avcodec_parameters_to_context(pAudioCodecCtx, pFormatCtx->streams[audio_index]->codecpar);
//     if (ret < 0)
//     {
//         printf("avcodec_parameters_to_context failed \n");
//         return -1;
//     }


//     //查找视频解码器
//     const AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
//     if (!pCodec)
//     {
//         printf("avcodec_find_decoder failed \n");
//         return -1;
//     }
//     //查找音频解码器
//     const AVCodec *pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
//     if (!pAudioCodec)
//     {
//         printf("avcodec_find_decoder failed \n");
//         return -1;
//     }


//     //打开视频解码器，将解码器和解码器上下文关联
//     ret = avcodec_open2(pCodecCtx, pCodec, NULL);
//     if (ret < 0)
//     {
//         printf("avcodec_open2 failed \n");
//         return -1;
//     }


//     //打开音频解码器，将解码器和解码器上下文关联
//     ret = avcodec_open2(pAudioCodecCtx, pAudioCodec, NULL);
//     if (ret < 0)
//     {
//         printf("avcodec_open2 failed \n");
//         return -1;
//     }
//     printf("-----------------------file open---------------------\n");
//     av_dump_format(pFormatCtx, 0, argv[1], 0);
//     printf("-----------------------file open---------------------\n");

//     AVPacket *packet=av_packet_alloc();
//     //av_init_packet(packet);


//     AVFrame *pFrame = av_frame_alloc();
//     AVFrame *pAudioFrame = av_frame_alloc();


//     while(g_quit!=1)
//     {
//         ret=av_read_frame(pFormatCtx, packet);
//         if(ret<0)
//         {
//             printf("av_read_frame end of  \n");
//             break;
//         }

//         if(packet->stream_index==video_index)//分离音视频包
//         {
//             printf("video packet \n");
//             //发送原始包
//             ret=avcodec_send_packet(pCodecCtx, packet);
//             if(ret!=0)
//             {
//                 printf("avcodec_send_packet failed  ret :%d\n",ret);
//                 return -1;
//             }
//             //读取解码后的包
//             ret=avcodec_receive_frame(pCodecCtx, pFrame);
//             if(ret!=0)
//             {
//                 printf("avcodec_receive_frame failed  ret :%d\n",ret);
//                 return -1;
//             }
//             //
//         }
//         else if(packet->stream_index==audio_index)
//         {
//             printf("audio packet \n");
//             ret=avcodec_send_packet(pAudioCodecCtx, packet);
//             ret=avcodec_receive_frame(pAudioCodecCtx, pAudioFrame);


//         }else
//         {
//             printf("other packet \n");
//         }
//         av_packet_unref(packet);//释放packet内存

//     }

//     printf("main finash\n");
//     return 0;

// }



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __STDC_CONSTANT_MACROS


#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"


#include "SDL.h"


#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

//Output PCM
#define OUTPUT_PCM 1
//Use SDL
#define USE_SDL 1

//Buffer:
//|-----------|-------------|
//chunk-------pos---len-----|
static  Uint8  *audio_chunk;
static  Uint32  audio_len;
static  Uint8  *audio_pos;

/* The audio function callback takes the following parameters:
 * stream: A pointer to the audio buffer to be filled
 * len: The length (in bytes) of the audio buffer
*/
void  fill_audio(void *udata,Uint8 *stream,int len){
    //SDL 2.0
    SDL_memset(stream, 0, len);
    if(audio_len==0)
        return;

    len=(len>audio_len?audio_len:len);	/*  Mix  as  much  data  as  possible  */

    SDL_MixAudio(stream,audio_pos,len,SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}
//-----------------

#undef main
int main(int argc, char* argv[])
{
    AVFormatContext	*pFormatCtx;
    int				i, audioStream;
    AVCodecContext	*pCodecCtx;
    const AVCodec			*pCodec;
    AVPacket		*packet;
    uint8_t			*out_buffer;
    AVFrame			*pFrame;
    SDL_AudioSpec wanted_spec;
    int ret;
    uint32_t len = 0;
    int got_picture;
    int index = 0;
    int64_t in_channel_layout;
    struct SwrContext *au_convert_ctx;

    FILE *pFile=NULL;
    char url[]="test.flv";

    //av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();
    //Open
    if(avformat_open_input(&pFormatCtx,url,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }
    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        printf("Couldn't find stream information.\n");
        return -1;
    }
    // Dump valid information onto standard error
    av_dump_format(pFormatCtx, 0, url, 0);

    // Find the first audio stream
    audioStream=-1;
    for(i=0; i < pFormatCtx->nb_streams; i++)
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            audioStream=i;
            break;
        }

    if(audioStream==-1){
        printf("Didn't find a audio stream.\n");
        return -1;
    }

    // Get a pointer to the codec context for the audio stream
    // pCodecCtx=pFormatCtx->streams[audioStream]->codecpar;

    pCodecCtx = avcodec_alloc_context3(NULL);
    //pCodecCtx = pFormatCtx->streams[videoindex]->codecpar;
    ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[audioStream]->codecpar);




    // Find the decoder for the audio stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL){
        printf("Codec not found.\n");
        return -1;
    }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){
        printf("Could not open codec.\n");
        return -1;
    }


#if OUTPUT_PCM
    pFile=fopen("output.pcm", "wb");
#endif

    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    //av_init_packet(packet);

    //Out Audio Param
    uint64_t out_channel_layout=AV_CH_LAYOUT_STEREO;
    //nb_samples: AAC-1024 MP3-1152
    int out_nb_samples=pCodecCtx->frame_size;
    printf("pCodecCtx->frame_size  %d ",pCodecCtx->frame_size);
    enum AVSampleFormat out_sample_fmt=AV_SAMPLE_FMT_S16;

    int out_sample_rate=48000;//48000/44100
    
    int out_channels=av_get_channel_layout_nb_channels(out_channel_layout);
    printf("out_channels\n ",out_channels);
    //Out Buffer Size
    int out_buffer_size=av_samples_get_buffer_size(NULL,out_channels ,out_nb_samples,out_sample_fmt, 1);


    out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);
    pFrame=av_frame_alloc();
//SDL------------------
#if USE_SDL
    //Init
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }
    //SDL_AudioSpec
    wanted_spec.freq = out_sample_rate;
    wanted_spec.format = AUDIO_S16SYS;
    wanted_spec.channels = out_channels;
    wanted_spec.silence = 0;
    wanted_spec.samples = out_nb_samples;
    wanted_spec.callback = fill_audio;
    wanted_spec.userdata = pCodecCtx;

    if (SDL_OpenAudio(&wanted_spec, NULL)<0){
        printf("can't open audio.\n");
        return -1;
    }
#endif

    in_channel_layout=av_get_default_channel_layout(pCodecCtx->channels);
    //in_channel_layout=av_get_default_channel_layout_fallback(pCodecCtx->channels)
    printf("in_channel_layout %d\n ",in_channel_layout);


    au_convert_ctx = swr_alloc();
    //  au_convert_ctx=swr_alloc_set_opts(au_convert_ctx,out_channel_layout, out_sample_fmt,out_sample_rate,in_channel_layout,pCodecCtx->sample_fmt,pCodecCtx->sample_rate,0, NULL);



    // 设置输入参数
    av_opt_set_int(au_convert_ctx, "in_channel_layout", in_channel_layout, 0);
    av_opt_set_int(au_convert_ctx, "in_sample_rate", pCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(au_convert_ctx, "in_sample_fmt", pCodecCtx->sample_fmt, 0);

    // 设置输出参数
    av_opt_set_int(au_convert_ctx, "out_channel_layout", out_channel_layout, 0);
    av_opt_set_int(au_convert_ctx, "out_sample_rate", out_sample_rate, 0);
    av_opt_set_sample_fmt(au_convert_ctx, "out_sample_fmt", out_sample_fmt, 0);

    // 初始化SwrContext
    ret = swr_init(au_convert_ctx);
    if (ret < 0) {
        printf("Failed to initialize SwrContext.\n");
        swr_free(&au_convert_ctx);
        return -1;
    }


    swr_init(au_convert_ctx);

    //Play
    SDL_PauseAudio(0);

    while(av_read_frame(pFormatCtx, packet)>=0)
    {

        if(packet->stream_index==audioStream){
            //ret = avcodec_decode_subtitle2( pCodecCtx, pFrame,&got_picture, packet);
            ret=avcodec_send_packet(pCodecCtx,packet);
            if ( ret < 0 ) {
                printf("Error in decoding audio frame.\n");
                return -1;
            }
            ret=avcodec_receive_frame(pCodecCtx,pFrame);
            if ( ret < 0 ) {
                printf("Error in decoding audio frame.\n");
                return -1;
            }

            if ( got_picture > 0 ){
                swr_convert(au_convert_ctx,&out_buffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrame->data , pFrame->nb_samples);
                printf("index:%5d\t pts:%lld\t packet size:%d\n",index,packet->pts,packet->size);
#if OUTPUT_PCM \
                //Write PCM
                fwrite(out_buffer, 1, out_buffer_size, pFile);
#endif
                index++;
            }
#if USE_SDL
            while(audio_len>0)//Wait until finish
                SDL_Delay(1);

            //Set audio buffer (PCM data)
            audio_chunk = (Uint8 *) out_buffer;
            //Audio buffer length
            audio_len =out_buffer_size;
            audio_pos = audio_chunk;
#endif
        }
        av_packet_unref(packet);
    }

    swr_free(&au_convert_ctx);

#if USE_SDL
    SDL_CloseAudio();//Close SDL
    SDL_Quit();
#endif

#if OUTPUT_PCM
    fclose(pFile);
#endif
    av_free(out_buffer);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return 0;
}




