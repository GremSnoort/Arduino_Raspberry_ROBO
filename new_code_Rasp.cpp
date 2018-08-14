#include <cv.h>// подключение заголовочных файлов библиотеки OpenCV
#include <highgui.h>
#include <imgproc.hpp>
#include <video.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <SerialStream.h>

using namespace LibSerial ;

using namespace std;

using namespace cv;

char device[]= "/dev/ttyACM0";

SerialStream my_serial_stream;

IplImage* rgb = 0;

void MakeCom()
{
	my_serial_stream.Open(device) ;
	my_serial_stream.SetBaudRate( SerialStreamBuf::BAUD_115200 ) ;
	cout << "connect " << endl;
}

void SendToSerialMove(int m1, int m2, int t) 
{
	my_serial_stream << "|MOVE," << m1 << "," << m2 << "," << t << "|";
	usleep(2000);
}

void SendToSerialServa(int s1) 
{
	my_serial_stream << "|SERVA" << s1 << "|";
	usleep(2000);
}

int main(int argc, char* argv[])
{
	MakeCom();
	CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
	assert( capture );

	double width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);

	IplImage* frame=0;
	IplImage *gray=0;

	int k;

	while(true)
	{
		frame = cvQueryFrame( capture );
		gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		cvCvtColor(frame,gray,CV_RGB2GRAY);
		rgb = cvCreateImage( cvGetSize(gray), IPL_DEPTH_8U, 0 );
		cvCopy(gray, rgb);
		cvInRangeS(gray, cvScalar(0, 0, 0), cvScalar(65, 65, 65), rgb);

		int y=rgb->height-5;
		int a[rgb->width] = {0};

		int pos=-1;

		for( int x=0; x<=rgb->width; x++ )
		{
			k = CV_IMAGE_ELEM(rgb, uchar, y, x);

			if(k==255)
			{
				pos+=1; 
				a[pos]=x;
			}
		}

		if(pos!=-1)
		{
			int MIDDLE=(rgb->width)/2;
			int LEFT=MIDDLE-a[0];
			int RIGHT=a[pos]-MIDDLE;

			if(LEFT>80 && RIGHT>80){SendToSerialMove(-150,-140,25);}
			if(LEFT>40 && LEFT<80){SendToSerialMove(-75,-140,25);}
			if(RIGHT>40 && RIGHT<80){SendToSerialMove(-150,-75,25);}
			if(LEFT<40 && RIGHT>0){SendToSerialMove(0,-140,25);}//right turn
			if(LEFT>0 && RIGHT<40){SendToSerialMove(-150,0,25);}//left turn
		}

		char c = cvWaitKey(33);

		if (c == 27)
			break;

		//cvShowImage("rgb and", rgb); - показывает обрабатываемое изображение, нужна для отладки цветовой гаммы, используется при настройке кода при подключении к Raspberry монитора
	}

	cvReleaseCapture( &capture );
	cvReleaseImage(& frame);
	cvReleaseImage(& gray);
	cvReleaseImage(& rgb);
	//cvDestroyAllWindows();

	return 0;
}