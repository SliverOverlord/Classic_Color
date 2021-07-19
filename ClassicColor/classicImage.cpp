/*
Author: Joshua DeNio
Date: 12/29/2020
Description: This program uses multithreading to convert a color image to a black and white image.


To compile: g++ -O3 -w classicImage.cpp -lpthread -o classicImage
To run: ./classicImage
*/


#include "stb_image.c"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <pthread.h>

using namespace std;

int thread_count;
const int MAX_LEN = 100;
uint8* src;
uint8* dst;
int width = 0;
int height = 0;
int actual_comps = 0;
int YR = 19595, YG = 38470, YB = 7471;


//Function declairations

//Name: convertPixelToGray
//Description: Converts a single pixel to grayscale
//Input: X,Y (ints)
//Output: none
//Precondition: There must be a valid image
//Postcondition: The pixel at x,y will be changed.
void convertPixelToGray(int y, int x);

//Name: Thread_convertToGray
//Description: Calls convert2gray on all pixels.
//Input: void* rank
//Output: void*
//Precondition: Must have a valid image
//Postcondition: Image converted to black and white
void* Thread_convertToGray(void* rank);

//End function declairations

//Functions

void convertPixelToGray(int y, int x){
    int index = (y*width+x)*actual_comps;
    int red = src[index];
    int green = src[index+1];
    int blue = src[index+2];

    //Get a weighted average of the three colors as the gray value.
    int avg = (red* YR + green * YG + blue * YB + 32768) / 65536;

    //Set the destination index
    dst[index] = avg;
    dst[index+1] = avg;
    dst[index+2] = avg;

}

void* Thread_convertToGray(void* rank){

    //Each thread will do some of the rows (number of rows is height).

    // cast the rank to a long
    long my_rank = (long) rank;

    long long my_n = height/thread_count;

    // Start and end possitions for each thread
    int my_start = my_n * my_rank;
    int my_end = (my_start + my_n);

    // If the data can be evenly spread spread it or send the extra to the last thread
    if (my_rank == thread_count - 1)
    {
        my_end = (height % thread_count) + my_end ;
    }

    for (int y = my_start; y < my_end; y++)
    {
        for(int x = 0; x < width; x++)
        {
            convertPixelToGray(y, x);
        }
    }
}

//End functions





//Main
int main(int argc, char** argv)
{
    // Declarations for threads.
    long thread;
    string path;
    string flag = "y";
    pthread_t* thread_handles;
    
    char* src_file;
    char* dst_file;

    //string* src_file;
    //string* dst_file;

    string str_src_file;
    string str_dst_file;
    thread_count = 4;

    // Give memory to the thread handles
    thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));

    // Load the source image.
    const int req_comps = 3; // request RGB image

    
    while (flag == "Y" || flag == "y") {

        //cout << "Enter the path to the desired file:\n";
        //cin >> path;


        cout << "Enter the filename to be converted: ";
        //cin >> str_src_file;
        src_file = new char[MAX_LEN];
        //fgets(src_file, MAX_LEN, stdin);
        cin >> src_file;

        cout << "Enter the new filename to be created: ";
        //cin >> str_dst_file;
        dst_file = new char[MAX_LEN];
        //fgets(dst_file, MAX_LEN, stdin);
        cin >> dst_file;

        //src_file = &str_src_file;
        //dst_file = &str_dst_file;

        src = stbi_load(src_file, &width, &height, &actual_comps, req_comps);
        if (!src)
        {
            cout<<"Failed loading file "<<endl;
            
        }
        else
        {
            cout<<"Source file: "<< src_file <<" "<< width <<" " <<height <<" "<< actual_comps<<endl;
            cout<<"RGB for pixel 0 (0,0) ::: "<<(int) src[0]<< " "<< (int) src[1]<< " "<< (int) src[2]<<endl;

            //Allocate memory for the output image. 
            dst = (uint8*) malloc(width* height*3);

            // Start threads

            for (thread = 0; thread < thread_count; thread++)
            {
                pthread_create(&thread_handles[thread], NULL, Thread_convertToGray, (void*)thread);
            }

            // Join the threads
            for (thread = 0; thread < thread_count; thread++)
            {
                pthread_join(thread_handles[thread], NULL);
            }

            // End of threads
  
            if (!stbi_write_tga(dst_file, width, height, 3, dst))
            {
                cout<<"Failed writing image to file "<< dst_file <<endl;
                free(src);
                free(dst);
                return EXIT_FAILURE;
            }

        }
        
        free(src);
        free(dst);

        cout << "Enter 'y' to convert another image, any other key to exit: ";
        cin >> flag;
    }

    return 0;
}
