#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
int main()
{
	printf("ffffffffffffff %s\n",avcodec_configuration());
	av_register_all();
	AVFormatContext *context = NULL;
	//const char *path = "test.mp4";
	const char *path = "http://39.134.65.162/PLTV/88888888/224/3221225611/index.m3u8";
	AVDictionary *ops = NULL;
	av_dict_set(&ops,"rtsp_transport","tcp",0);
	av_dict_set(&ops,"max_delay","550",0);
	int ret = avformat_open_input(&context,path,NULL,NULL);
	if(ret)
	{
		printf("avformat_open_input failed.\n");
		return -1;
	}
	printf("avformat_open_input successful.\n");
	ret = avformat_find_stream_info(context,NULL);
	if(ret)
	{
		printf("avformat_find_stream_info failed.\n");
		return -1;
	}
	printf("avformat_find_stream_info successful.\n");
	int minutes = context->duration/1000000/60;
	int seconds = context->duration/1000000%60;
	printf("%d minute %d second\n",minutes,seconds);
	av_dump_format(context,0,path,0);
	
	return 0;
}