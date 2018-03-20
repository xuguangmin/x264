#include <stdint.h>   
#include <stdio.h>  
//#include <io.h>
//#include <process.h>  
#include <fcntl.h>  
#include <stdlib.h>  
#include <string.h> 

#include "x264.h" 
#define IMAGE_WIDTH   320  
#define IMAGE_HEIGHT  240  
#define CLEAR(x) (memset((&x),0,sizeof(x)))  

char *read_filename="yuv420p.yuv";  
char *write_filename="x264yuv420p2.h264"; 

int  main(int argc ,char **argv){

	x264_param_t m_param;

	x264_param_default(&m_param);

	m_param.i_width = 320;
	m_param.i_height = 240;

	m_param.i_fps_num = 4;
	m_param.i_fps_den = 1;

	m_param.rc.i_bitrate = 96;
	m_param.rc.i_rc_method = X264_RC_ABR;

	//m_param.i_frame_reference = 4; /* 参考帧的最大帧数 */

	m_param.b_annexb = 1;

	m_param.i_keyint_max = 2;

	x264_t* m_h = NULL;

	 /* 根据输入参数param初始化总结构 x264_t */
	 if( ( m_h = x264_encoder_open( &m_param ) ) == NULL )
	 {
		 fprintf( stderr, "x264 [error]: x264_encoder_open failed\n" );
		 return -1;
	 }

	   x264_picture_t m_pic;
	   x264_picture_alloc( &m_pic, X264_CSP_I420, m_param.i_width, m_param.i_height );
	   m_pic.i_type = X264_TYPE_AUTO;
	   m_pic.i_qpplus1 = 0;

	   unsigned char *yuv;

	   yuv=(uint8_t *)malloc(IMAGE_WIDTH*IMAGE_HEIGHT*3/2);

	   CLEAR(*yuv);

	   m_pic.img.plane[0]=yuv;  
	   m_pic.img.plane[1]=yuv+IMAGE_WIDTH*IMAGE_HEIGHT/4;  
	   m_pic.img.plane[2]=yuv+IMAGE_WIDTH*IMAGE_HEIGHT+IMAGE_WIDTH*IMAGE_HEIGHT/4; 

	   int fd_read,fd_write; 

	   if((fd_read=open(read_filename,O_RDONLY))<0){  
		   printf("cannot open input file!\n");  
		   exit(EXIT_FAILURE);  
	   }  

	   if((fd_write=open(write_filename,O_WRONLY | O_APPEND | O_CREAT,0777))<0){  
		   printf("cannot open output file!\n");  
		   exit(EXIT_FAILURE);  
	   }  

	   int n_nal; 
	   int ret;
	   x264_picture_t pic_out;  
	   x264_nal_t *my_nal; 

	   x264_nal_t * nal;
	   nal=(x264_nal_t *)malloc(sizeof(x264_nal_t ));
	   CLEAR(nal);  

	   while(read(fd_read,yuv,IMAGE_WIDTH*IMAGE_HEIGHT*3/2)>0){  
		   
		   m_pic.i_pts++;  

		   if((ret=x264_encoder_encode(m_h,&nal,&n_nal,&m_pic,&pic_out))<0){  
			   printf("x264_encoder_encode error!\n");  
			   exit(EXIT_FAILURE);  
		   }  

		   unsigned int length=0;  
		   for(my_nal=nal;my_nal<nal+n_nal;++my_nal){  
			   write(fd_write,my_nal->p_payload,my_nal->i_payload);  
			   length+=my_nal->i_payload;  
		   }  
		   printf("length=%d\n",length);  
	   }  

	   free(yuv);  
	   free(&m_pic);  
	   free(&m_param);  
	   x264_encoder_close(m_h);   
	   close(fd_read);  
	   close(fd_write); 
	   return 0;
}
