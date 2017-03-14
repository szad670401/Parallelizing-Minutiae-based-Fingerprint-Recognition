#include <iostream>
#include <cmath>
#include <cstdint>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#define SIZE 11	// (SIZE x SIZE)should be odd


using namespace std;
using namespace cv;



/*
 * function to perform thinning of the binary image
 */
void thinning(const Mat& src, Mat& dst);


/*
 * function to compute orientation of the pixels
 */
Mat compute_orientation(const Mat matrix);


/*
 * main function
 */
int main()
{
	Mat img_src, img_norm, img_blur, img_hist, img_mat, img_thresh, img_thin;
	Mat orientation, new_orientation;
	vector<Mat> img_blk;
	Scalar mean, stddev;
	int r = 0, c = 0; 
		

	// read fingerprint image
	img_src = imread("../f0001_01.png", CV_LOAD_IMAGE_GRAYSCALE);


	// convert original image to a double precision matrix
	img_src.convertTo(img_mat, CV_64FC1);

	
	// add padding to the matrix
	Mat img_pad(img_mat.rows + SIZE - 1, img_mat.cols + SIZE - 1, CV_64FC1);
	copyMakeBorder(img_mat, img_pad, (SIZE-1)/2, (SIZE-1)/2, (SIZE-1)/2, (SIZE-1)/2, BORDER_REPLICATE);


	// get blocks of size 11x11 from the padded matrix
	for (int y = 0; y < img_pad.rows - SIZE + 1; y++)
	{
		for (int x = 0; x < img_pad.cols - SIZE + 1; x++)
		{
                        img_blk.push_back(img_pad(Rect(x, y, SIZE, SIZE)).clone());
		}
	}


	// obtain mask based on standard deviation
	Mat mask_mat(img_mat.rows, img_mat.cols, CV_64FC1);
	for(int i = 0; i < img_blk.size(); i++)
	{
		meanStdDev(img_blk[i], mean, stddev, Mat());

		if(stddev[0] > 10)
		{
			mask_mat.at<double>(r, c) = 1.0;
		}
		
		else
		{
			mask_mat.at<double>(r, c) = 0.0;
		}
		
		c++;
		if(c == mask_mat.cols)
		{
			c=0;
			r++;
		}
	}


	// convert mask matrix to grayscale image
	Mat mask(img_mat.rows, img_mat.cols, CV_8UC1);
	mask_mat.convertTo(mask, CV_8UC1, 255, 0);


	// perform morphological operations on the mask
	Mat const structure_elem1 = getStructuringElement(MORPH_ELLIPSE, Size(4, 4));
	Mat const structure_elem2 = getStructuringElement(MORPH_ELLIPSE, Size(14, 14));
	Mat open_result, close_result;
	morphologyEx(mask, open_result, MORPH_OPEN, structure_elem1);
	morphologyEx(open_result, close_result, MORPH_CLOSE, structure_elem2);
	


	//imshow("mask ready!", close_result);
	//TODO normalize the image - using close_result and 255 ; thus get img_norm



#if 0
	// perform gaussian blurring of the normalized image
	GaussianBlur(img_norm, img_blur, Size(3, 3), 0, 0);


	// perform histogram equalization of the blurred image
	equalizeHist(img_blur, img_hist);


	// obtain binary image from the equalized image
	threshold(img_hist, img_thresh, 100, 255, CV_THRESH_BINARY);


	// perform thinning of the binary image
	thinning(img_thresh, img_thin);
#endif


	// compute orientation of the pixels	
	orientation = compute_orientation(img_mat);


#if 0	
	//TODO read/write orientations to/from file
	// write:
	FileStorage fs1("f0001_01.xml", FileStorage::WRITE);
	fs1 << "f0001_01" << orientation;

	// read:
	FileStorage fs2("f0001_01.xml", FileStorage::READ);
	fs2["f0001_01"] >> new_orientation;
#endif	

	
	//imshow("fingerprint", img_thin);
	waitKey(0);

	return 0;
}

