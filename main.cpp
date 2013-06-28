#include <iostream>
#include <cmath>
#include <list>
#include <map>
#include <opencv2/opencv.hpp>

#define NO_BACKGROUND

using namespace cv;

typedef struct letter
{
	char character;
	double brightness;
};

double average_brightness( Mat &matrix )
{
	double brightness = 0;
	
	for( int x = 0; x < matrix.cols; x++ )
	{
		for( int y = 0; y < matrix.rows; y++ )
		{
			Vec3b v = matrix.at<Vec3b>( x, y ); // (RGB)
			
			brightness += ( ( v.val[ 0 ] + v.val[ 1 ] + v.val[ 2 ] ) / 3 );
		}
	}
	
	return brightness / ( matrix.cols * matrix.rows );
}

Scalar average_color( Mat &matrix )
{
	int width = matrix.cols;
	int height = matrix.rows;
	
	double color_red = 0;
	double color_green = 0;
	double color_blue = 0;
	
	for( int x = 0; x < width; x++ )
	{
		for( int y = 0; y < height; y++ )
		{
			Vec3b v = matrix.at<Vec3b>( x, y ); // (RGB)
			
			color_red += v.val[ 0 ];
			color_green += v.val[ 1 ];
			color_blue += v.val[ 2 ];
		}
	}
	
	double total_pixels = width * height;
	return CV_RGB( color_red / total_pixels, color_green / total_pixels, color_blue / total_pixels );	
}

std::map<double,char> generate_character_table( char characters[] )
{
	std::list<letter> temp;
	std::map<double,char> character_table;

	double min_brightness = INFINITY;
	double max_brightness = -1;
	
	// Generate character images and calculate brightness
	
	for( int i = 0; i < strlen( characters ); i++ )
	{
		Mat pic( 12, 7, CV_8UC3 );
		letter new_letter;
		
	   	pic.setTo( Scalar( 255, 255, 255 ) );
		addText( pic, &characters[ i ], Point( 0, 10 ), fontQt( "Courier New", -1, 1 ) ); // less than optimal but declaring outside the loop didn't work

		new_letter.brightness = average_brightness( pic );
		new_letter.character = characters[ i ];
		
		if( new_letter.brightness > max_brightness )
			max_brightness = new_letter.brightness;
		if( new_letter.brightness < min_brightness )
			min_brightness = new_letter.brightness;
		
		temp.push_back( new_letter );
	}
	
	// Normalise brightness
	
	for( std::list<letter>::const_iterator it = temp.begin(); it != temp.end(); ++it )
	{
		double normalised_brightness = 255 * ( it->brightness - min_brightness ) / ( max_brightness - min_brightness );
		character_table[ normalised_brightness ] = it->character;
	}
	
	return character_table;
}

int main( int argc, char *argv[] )
{
	cvNamedWindow( "output_window", CV_WINDOW_AUTOSIZE );
	
	// Retrieve input file
	
	if( argc < 2 )
	{
		std::cout << "Input file not specified." << std::endl;
		return 1;
	}
	
	Mat input_image = imread( argv[ 1 ], CV_LOAD_IMAGE_COLOR );
	
	if( !input_image.data )
	{
		std::cout << "Error opening image." << std::endl;
		return 1;
	}

	// Create lookup table for available characters
	char ascii_characters[] = "`1234567890-=qwertyuiop[]\asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?";
	std::map<double,char> character_table = generate_character_table( ascii_characters );

	// need to know background color for image generally so correct color appears behind text
	// Either use standard white background or average total picture color
	
	#ifdef NO_BACKGROUND
		Scalar background_color = average_color( input_image );
	#endif
	
	// Sample input image and compare to ascii images
	
	int x, y;
	for( x = 0; ( x + 7 ) <= input_image.cols; x += 7 )
	{
		for( y = 0; ( y + 12 ) <= input_image.rows; y += 12 )
		{
			Mat sample( input_image, Rect( x, y, 7, 12 ) );
			double sample_brightness = average_brightness( sample );
			char match = character_table.lower_bound( sample_brightness )->second;			
			Scalar color = average_color( sample );
			
#ifdef NO_BACKGROUND
			sample.setTo( Scalar( 255, 255, 255 ) );
#else
			sample.setTo( background_color );
#endif
			
			addText( sample, &match, Point( 0, 10 ), fontQt( "Courier New", -1, color ) ); // less than optimal but declaring outside the loop didn't work
		}
	}
	
	// Clean up parts of the image that could not be filled (due to gaps being < 10px)
	// using left over x/y values from the for loop
	
	if( x <= input_image.cols )
	{
		Mat i( input_image, Rect( Point( x, 0 ), Point( input_image.cols, input_image.rows ) ) );
		i.setTo( Scalar( 255, 255, 255 ) );
	}	
	if( y <= input_image.rows )
	{
		Mat i( input_image, Rect( Point( 0, y ), Point( x, input_image.rows ) ) );								 
		i.setTo( Scalar( 255, 255, 255 ) );
	}
	
	imshow( "output_window", input_image );
	cvWaitKey(0);
	
	return 0;
	
}
