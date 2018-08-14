#include <cv.h>
#include <highgui.h>
#include <imgproc.hpp>
#include <video.hpp>
#include <stdlib.h>
#include <stdio.h>
//#include <chrono>
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

void SendToSerialMove(int m1, int m2, int t) {
    my_serial_stream << "|MOVE," << m1 << "," << m2 << "," << t  << "|";
    usleep(2000);
}
void SendToSerialServa(int s1) {
    my_serial_stream << "|SERVA" << s1 <<  "|";
    usleep(2000);
}

int main(int argc, char* argv[])
{

	MakeCom();

//SendToSerialServa(90);
//SendToSerialMove(1000,0,1050);


      CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
	assert( capture );  
	double width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
        double height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);

	IplImage* frame=0;
	IplImage *gray=0;

int k;

        
	cvNamedWindow("rgb and",CV_WINDOW_AUTOSIZE);

		while(true){
                
                frame = cvQueryFrame( capture );
		
		gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
		cvCvtColor(frame,gray,CV_RGB2GRAY);
    
		rgb = cvCreateImage( cvGetSize(gray), IPL_DEPTH_8U, 0 );
		cvCopy(gray, rgb);

		cvInRangeS(gray, cvScalar(0, 0, 0), cvScalar(60, 60, 60), rgb);
                


int tmp=0;
int FIRST=0;
int LAST=0;
int first_part=0;
int second_part=0;
int half = (rgb->width)/2;
                for(int y=((rgb->height)/4)*3; y<rgb->height; y++ )
		{int v=0;

			
			tmp=0;
			for( int x=0; x<rgb->width; x++ ) 
			{
				
				
				
				k = CV_IMAGE_ELEM(rgb, int, y, x);
				if(k==255 && tmp==0){FIRST=x;tmp=1;}
				if(k==0 && tmp==1){LAST=x;tmp=0;}
				
				
				
					if(FIRST<LAST){
					first_part=half-FIRST;
					second_part=LAST-half;
					v=first_part/second_part;
					//cout<<FIRST<<"  "<<half<<"  "<<LAST<<endl;
					
					}

				
			}

			if(v>0)
			{cout<<"See"<<endl;
				int lev=500;
				int prav=lev*second_part/first_part;
				SendToSerialMove(lev,prav,100);

			}


		}
        
                char c = cvWaitKey(33);
                if (c == 27) { // если нажата ESC - выходим
                        break;
                }


		
		
		cvShowImage("rgb and", rgb);
   
		}

	
	cvReleaseCapture( &capture );
        cvReleaseImage(& frame);
	cvReleaseImage(& gray);
	cvReleaseImage(& rgb);
	cvDestroyAllWindows();
        return 0;
}