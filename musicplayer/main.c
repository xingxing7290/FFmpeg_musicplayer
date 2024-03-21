

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
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

int is_paused = 0; // 0表示播放，1表示暂停
/* The audio function callback takes the following parameters:
 * stream: A pointer to the audio buffer to be filled
 * len: The length (in bytes) of the audio buffer
*/
void  fill_audio(void *udata,Uint8 *stream,int len){
    //SDL 2.0
    SDL_memset(stream, 0, len);
    // if(audio_len==0)
    //     return;
    if (audio_len == 0 || is_paused) {
        return;
    }

    len=(len>audio_len?audio_len:len);	/*  Mix  as  much  data  as  possible  */

    SDL_MixAudio(stream,audio_pos,len,SDL_MIX_MAXVOLUME);
    audio_pos += len;
    audio_len -= len;
}

//返回值是us
unsigned int  GetTimeInterval(struct timeval start, struct timeval stop)
{
    unsigned int TimeUs;
    if(stop.tv_sec == start.tv_sec)
    {
        TimeUs =stop.tv_usec - start.tv_usec;
    }
    else if(stop.tv_sec > start.tv_sec)
    {
        if(stop.tv_usec < start.tv_usec)
        {
            TimeUs = (stop.tv_sec - 1 - start.tv_sec)* 1000000 + (stop.tv_usec + 1000000 - start.tv_usec);
        }
        else
        {
            TimeUs = (stop.tv_sec - start.tv_sec)* 1000000 + (stop.tv_usec - start.tv_usec);
        }
    }
    else
    {
        TimeUs = 0;
    }
    return TimeUs;
}


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
    char url[]="C:/Users/Administrator/Documents/ffmpeg/test2.mp4";

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
     int videoStream=-1;
    for(i=0; i < pFormatCtx->nb_streams; i++){
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO){
            audioStream=i;
            break;
        }
        if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoStream=i;
            break;
        }
    }

    if(audioStream==-1){
        printf("Didn't find a audio stream.\n");
        return -1;
    }

         // 计算帧率
    double fps = av_q2d(pFormatCtx->streams[videoStream]->avg_frame_rate);

    // 计算总帧数
    int64_t total_frames = (pFormatCtx->duration / AV_TIME_BASE) * fps;

    printf("Total frames: %lld\n", total_frames);


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
    printf("-----------------------file open---------------------\n");
    av_dump_format(pFormatCtx, 0, argv[1], 0);



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

    // 获取音频流的采样率
    int sample_rate =pFormatCtx->streams[audioStream]->codecpar->sample_rate;
    printf("sample_rate%d Hz\n", sample_rate);

    int out_sample_rate=sample_rate;//48000/44100

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
    struct timeval startime , endtime;

    SDL_Event event;

    while(av_read_frame(pFormatCtx, packet)>=0)
    {
        gettimeofday(&startime,NULL);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    is_paused = !is_paused; // 切换播放/暂停状态
                    if (is_paused) {
                        printf("Paused\n");
                    } else {
                        printf("Resumed\n");
                    }
                }
            }
        }
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
                //重采样函数
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

            gettimeofday(&endtime,NULL);

            int gap =GetTimeInterval(startime,endtime);
            printf("GetTimeInterval %d    ",gap);
            // if (gap <= 40000) {
            //    usleep (gap);
            //    printf("GetTimeInterval %d    ", 40 - gap);
            // }

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




