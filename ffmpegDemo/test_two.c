#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

int main()
{
	int ret,got_picture;
	FILE *fileIn = NULL;
	FILE *fileOut = NULL;
	AVCodec *codec = NULL;
	AVCodecContext *codecContext = NULL;
	AVCodecParserContext *parseContext = NULL;
	struct SwsContext *swsContext = NULL;
	enum AVCodecID codecId = AV_CODEC_ID_H264;
	AVFrame *frame = NULL,*frameYuv = NULL;
	uint8_t *curPtr = NULL;
	AVPacket packet;
	const int buffSize = 4096;
	int curSize,firstTime = 1,ySize;
	uint8_t buffIn[buffSize+FF_INPUT_BUFFER_PADDING_SIZE],*buffOut = NULL;
	char *filePathIn = "bigbuckbunny_480x272.h264";
	char *filePathOut = "bigbuckbunny_480x272.yuv";

	avcodec_register_all();
	codec = avcodec_find_decoder(codecId);
	if(!codec){
		printf("avcodec_find_decoder failed.\n");
		return -1;
	}
	codecContext = avcodec_alloc_context3(codec);
	if(!codecContext){
		printf("avcodec_alloc_context3 failed.\n");
		return -1;
	}
	parseContext=av_parser_init(codecId);
	if (!parseContext){
		printf("Could not allocate video parser context\n");
		return -1;
	}
	if(codec->capabilities | CODEC_CAP_TRUNCATED)
		codecContext->flags |= CODEC_FLAG_TRUNCATED;

	ret = avcodec_open2(codecContext,codec,NULL);
	if(ret < 0){
		printf("avcodec_open2 failed.\n");
		return -1;
	}
	fileIn = fopen(filePathIn,"rb");
	if(!fileIn){
		printf("open %s failed.\n",filePathIn);
		return -1;
	}
	fileOut = fopen(filePathOut,"wb");
	if(!fileOut){
		printf("open %s failed.\n",filePathOut);
		return -1;
	}

	frame = av_frame_alloc();
	if(!frame){
		printf("alloc frame failed;\n");
		return -1;
	}
	av_init_packet(&packet);
	memset(buffIn,0,4096+FF_INPUT_BUFFER_PADDING_SIZE);
	while(1){
		curSize = fread(buffIn,1,buffSize,fileIn);
		if(curSize == 0)
			break;
		curPtr = buffIn;
		while(curSize > 0){
			int len = av_parser_parse2(parseContext,codecContext,&packet.data,&packet.size,curPtr,curSize,AV_NOPTS_VALUE,AV_NOPTS_VALUE,AV_NOPTS_VALUE);
			curPtr += len;
			curSize -= len;
			if(0 == packet.size)
				continue;
			printf("Packet Size:%6d\t",packet.size);
			switch(parseContext->pict_type){
				case AV_PICTURE_TYPE_I:printf("Type: I\t");break;
				case AV_PICTURE_TYPE_B:printf("Type: B\t");break;
				case AV_PICTURE_TYPE_P:printf("Type: P\t");break;
				default: printf("Type: Other\t");break;
			}
			printf("Output Number:%4d\t",parseContext->output_picture_number);
			printf("Offset:%8ld\n",parseContext->cur_offset);

			ret = avcodec_decode_video2(codecContext,frame,&got_picture,&packet);
			if(ret < 0){
				printf("decode error.\n");
				return ret;
			}
			if(got_picture){
				if(firstTime){
					printf("Codec Full Name %s\n",codecContext->codec->long_name);
					printf("width : %d ,hwight : %d\n",codecContext->width,codecContext->height);
					swsContext = sws_getContext(codecContext->width,codecContext->height,codecContext->pix_fmt,codecContext->width,codecContext->height,AV_PIX_FMT_YUV420P,SWS_BICUBIC, NULL, NULL, NULL);
					frameYuv = av_frame_alloc();
					buffOut = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P,codecContext->width,codecContext->height));
					avpicture_fill((AVPicture *)frameYuv,buffOut,AV_PIX_FMT_YUV420P,codecContext->width,codecContext->height);
					ySize = codecContext->width*codecContext->height;
					firstTime = 0;
				}
				printf("success decode 1 frame.\n");
				sws_scale(swsContext,(const uint8_t* const*)frame->data,frame->linesize,0,
					codecContext->height,frameYuv->data,frameYuv->linesize);
				fwrite(frameYuv->data[0],1,ySize,fileOut);
				fwrite(frameYuv->data[1],1,ySize/4,fileOut);
				fwrite(frameYuv->data[2],1,ySize/4,fileOut);
			}
		}
		
	}




	fclose(fileIn);
	fclose(fileOut);
	sws_freeContext(swsContext);
	av_parser_close(parseContext);
	av_frame_free(&frameYuv);
	av_frame_free(&frame);
	avcodec_close(codecContext);
	av_free(codecContext);
	return 0;
}
