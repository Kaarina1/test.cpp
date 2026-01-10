#include <iostream>
#include <mpi.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <vector>
#include <dirent.h>
#include <string>

using namespace cv;

int count_images() {
    DIR* FD;        //Pointer for a directory
    struct dirent* image;   //Pointer to an image
    const char* target_dir = "/mnt/shared/cpp.test/images/animals"; //Image file path
	int image_count = 0; 
    FD = opendir(target_dir); //Pointing to directory
    if (FD == NULL) //If there is no directory
    {
        fprintf(stderr, "Error: Failed to open input directory - %s\n", strerror(errno));
        return 1;
    }

    /* Reading object (files, directories ...) from the folder */
    while ((image = readdir(FD))) //For all files in directory
    {
		if(strcmp(image->d_name, ".")==0|| strcmp(image->d_name, "..")==0){
			continue;
		}
		else{
			image_count++;
		}
    }

    /* Close the directory */
    closedir(FD);

    return image_count;
}

Mat rotate_image(const Mat& original_image){
	int random_number = rand() % 91;
	int Height = original_image.rows / 2;//getting middle point of rows//
    int Width = original_image.cols / 2;//getting middle point of height//
	Mat rotation_matrix = getRotationMatrix2D(Point(Width, Height), random_number, 1);//affine transformation matrix for 2D rotation//
    Mat new_image;//declaring a matrix for rotated image
    warpAffine(original_image, new_image, rotation_matrix, original_image.size());//applying affine transformation//

	return new_image;
		  
}

Mat blur_image(const Mat& original_image){
	int blur_array[6] = {1,3,5,7,9,11};
	int random_number = rand() % 7;
	int blur_value = blur_array[random_number];
	Mat new_image = GaussianBlur(original_image,(blur_value,blur_value),0);

	return new_image;
}

void alter_image(const std::string& image_path_in,const std::string& image_path_out){
	Mat image_data = imread(image_path_in);//get image

	Mat resized_image = resize(image_data, (300,300));

	Mat blurred_image = blurred_image(resized_image);
	
	Mat rotated_image = rotate_image(blurred_image);
	
	imwrite(image_path_out,rotated_image);//save new image
}

void process_images(int start, int image_amount){
	DIR* FD;        //Pointer for a directory
    struct dirent* image;   //Pointer to an image
    const char* target_dir = "/mnt/shared/cpp.test/images/animals"; //Image file path
	FD = opendir(target_dir); //Pointing to directory
	int image_pointer=0; //For image place
	int end = start+image_amount;
	std::vector<std::string> image_names;
	if (FD == NULL) //If there is no directory
    {
        fprintf(stderr, "Error: Failed to open input directory - %s\n", strerror(errno));
    }
	else{
		while ((image = readdir(FD)) && image_pointer<end) //For all files in directory
	    {
			//Skip directories
			if(strcmp(image->d_name, ".")==0|| strcmp(image->d_name, "..")==0){
				continue;
			}
			else if(image_pointer<start){
				image_pointer++;
			}
			else{
				image_names.push_back(image->d_name);
				for(std::string image_name : image_names){
					std::string image_path_in = std::string(target_dir) + "/" + image_name;
					std::string image_path_out = "/mnt/shared/cpp.test/images/new/" + image_name;
					alter_image(image_path_in,image_path_out);
				}
				image_pointer++;
			}
	    }
	}
    /* Close the directory */
    closedir(FD);
}

int main(int argc, char** argv)
{	//INITIALISE MPI
	MPI_Init(&argc, &argv);
	//Get MPI Variables
		// GET Number of processes (MPI_COMM_size)
	int process_rank, number_of_processes;	
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
		// Get Rank of process (MPI_COMM_rank)
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);

	int image_count;
	int images_per_process;
	int percent_input = std::stoi(argv[1]);
	printf("You entered %d \n",percent_input);
	
	//IF Master:
	if (process_rank == 0) {
		//Request image count
		image_count = count_images();
		printf("There are %d images\n", image_count);
		//CALCULATE batch size
		images_per_process = image_count/(number_of_processes-1);
	}
	//SEND Workers Rank and batch size
	MPI_Bcast(&images_per_process,1,MPI_INT,0,MPI_COMM_WORLD);	
	//For each worker
	if(process_rank!=0){
		//Calculate the percentage to process
		int image_amount;
		image_amount = images_per_process*percent_input/100;
		//Calculate the start pointer value
		int start = (process_rank-1)*(image_amount);
		
		process_images(start, image_amount);
		printf("I am rank %d. I processed %d images\n", process_rank ,images_per_process);
	}
	//FINALISE MPI
	return MPI_Finalize();
}
