#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace cv;

// Treat matrix as vector and calculate magnitude
double matrix_magnitude( Mat &matrix )
{
	double sum = 0;
	
	std::cout << matrix << std::endl;
	
	for( int x = 0; x < matrix.cols; x++ )
	{
		for( int y = 0; y < matrix.rows; y++ )
		{
			sum += pow( (double)matrix.at<uchar>( x, y ), 2.0 );
			std::cout << (double)matrix.at<uchar>(x,y) << "+";
		}
	}
	
	std::cout << std::endl << sum << std::endl;
	std::cout << sqrt( sum ) << std::endl;
	
	return 1;
	
//	return sqrt( sum );
}

int main( int argc, char *argv[] )
{
	cvNamedWindow( "a", CV_WINDOW_AUTOSIZE );

	// Retrieve input file
	
	if( argc < 2 )
	{
		std::cout << "Input file not specified." << std::endl;
		return 1;
	}
	
	Mat input_image = imread( argv[ 1 ], CV_LOAD_IMAGE_GRAYSCALE );
	if( !input_image.data )
	{
		std::cout << "Error opening image." << std::endl;
		return 1;
	}

	// Create images for standard characters
	
	//	char ascii_values[] = "`1234567890-=qwertyuiop[]\asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
	char ascii_values[] = "`@";
	
	vector<Mat> ascii_images;
	
	Mat pic( 12, 7, CV_8UC3 );
	
	for( int i = 0; i < strlen( ascii_values ); i++ )
	{
	   	pic.setTo( Scalar( 255, 255, 255 ) );
		addText( pic, &ascii_values[ i ], Point( 0, 10 ), fontQt( "Courier New", -1, 1 ) ); // less than optimal but declaring outside the loop didn't work
		
		Mat new_pic;
		cvtColor( pic.clone(), new_pic, CV_RGB2GRAY ); // less than optimal but initially declaring as grayscale stops text from displaying
		ascii_images.push_back( new_pic );
//			imshow( "a", new_pic );
//			cvWaitKey(0);
	}

	// Sample input image and compare to ascii images
	
//	std::cout << input_image.cols << ", " << input_image.rows << std::endl;
	for( int x = 0; ( x + 8 ) < input_image.cols; x += 8 )
	{
		for( int y = 0; ( y + 13 ) < input_image.rows; y += 13 )
		{
//			std::cout << x << ", " << y << std::endl;

			// Obtain sample. Length/width of the rectangle are one less
			// than the x/y increment valuesto ensure borders are not 
			// double counted.
			Mat sample( input_image, Rect( x, y, 7, 12 ) );

			for( vector< Mat >::iterator it = ascii_images.begin(); it != ascii_images.end(); ++it )
			{
				matrix_magnitude( *it );
			}
		}
	}

//	imshow( "a", pic );
//	cvWaitKey(0);

	
	return 0;

}
