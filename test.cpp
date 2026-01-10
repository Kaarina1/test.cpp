#include <iostream>
#include <mpi.h>
#include "opencv2/imgproc.hpp"
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

void alter_image(const std::string& image_path){
	Mat image_data = imread(image_path);
	std::string image_path = std::string(target_dir) + "/" + image_name;
	imwrite(image_path,image_data);
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
				for(std::string image_name : image_names){
				std::string image_path = std::string(target_dir) + "/" + image_name;
				alter_image(image_path);
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
		int start = process_rank*(image_amount);
		
		process_images(start, image_amount);
		printf("I am rank %d. I processed %d images\n", process_rank ,images_per_process);
	}
	//FINALISE MPI
	return MPI_Finalize();
}
