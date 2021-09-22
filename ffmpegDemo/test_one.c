#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
int main()
{
	printf("ffffffffffffff %s\n",avcodec_configuration());
	av_register_all();
	AVFormatContext *context = NULL;
	const char *path = "test.mp4";
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
	
	return 0;
}