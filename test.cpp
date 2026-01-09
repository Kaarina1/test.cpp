#include <iostream>
#include <mpi.h>
#include "opencv2/imgproc.hpp"
#include <vector>
#include <dirent.h>

int count_images() {
    DIR* FD;        /* represent the directory */
    struct dirent* image;   /* represent the file */
    char* target_dir = "/mnt/shared/cpp.test/images"; /* current directory */
	int image_count = 0;
    /* Scanning the target directory */
    FD = opendir(target_dir);
    if (FD == NULL)
    {
        fprintf(stderr, "Error: Failed to open input directory - %s\n", strerror(errno));
        return 1;
    }

    /* Reading object (files, directories ...) from the folder */
    while ((image = readdir(FD)))
    {
		image_count++;
    }

    /* Close the directory */
    closedir(FD);

    return image_count;
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
	//IF Master:
	if (process_rank == 0) {
		//Open image folder
		
		int  = count_images();
		//COUNT image files
		printf("There are  - %d\n - number of images", );
			//ADD image file name to array
		//CLOSE image folder
		//CALCULATE batch size = Number of images/ Number of processes
		//SEND Workers Rank and batch size (MPI_Bcast)
	}
		
	
	//ELSE For each Worker
		//CALCULATE start = rank*batch size
		//FOR each image in batch given (from start to batch size)
			//RESIZE
			//DO n times
				//ADD random noise
				//ADD random blur
				// SAVE new image	
	//FINALISE MPI
}
