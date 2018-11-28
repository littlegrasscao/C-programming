#include "mnist.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <time.h>

struct mnist_dataset_t{
    int label_magic; //label magic number
    int image_magic; //image magic number
    int number_item;  //number of items    
    int number_image; //number of images
    unsigned int number_row;   //number of rows
    unsigned int number_col;   //number of columns
    mnist_image_handle head;   //pointer to the first image
};

struct mnist_image_t{
    unsigned char * image;                //store pixel array[] length of row*col
    unsigned char label;                  //label of image
    mnist_image_handle next;    //point to the next imgae
    mnist_image_handle previous;//pointer to the previous image
};

/// Open the given MNIST formatted dataset;
/// Returns MNIST_DATASET_INVALID if the dataset could not be opened,
/// and a mnist_dataset_handle on success.
mnist_dataset_handle mnist_open (const char * name){
    //combine name with the rest part
    char * name_label = malloc(strlen(name) + 20);
	strcpy(name_label, name);
	strcat(name_label, "-labels-idx1-ubyte");
	
    char * name_image = malloc(strlen(name) + 20);
	strcpy(name_image, name);
	strcat(name_image, "-images-idx3-ubyte");

    //open file. if cannot open, return MNIST_DATASET_INVALID
    FILE * f_image;
    if((f_image = fopen(name_image,"rb"))==NULL)
    {
	//free space for names
	free(name_image);
	free(name_label);        
	return MNIST_DATASET_INVALID;
    }
    FILE * f_label;
    if((f_label = fopen(name_label,"rb"))==NULL)
    {
    	//free space for names
    	free(name_image);
    	free(name_label);        
	return MNIST_DATASET_INVALID;
    }

    //free space for names
    free(name_image);
    free(name_label);

    //malloc space for handle
    mnist_dataset_handle handle = (mnist_dataset_handle)malloc(sizeof(struct mnist_dataset_t));

    //read the 'magic number'
    //fread(buffer,size,count,fp). read first 32 bits
    uint32_t tmp;
    fread(&tmp,sizeof(uint32_t),1,f_label);
    handle->label_magic = ntohl(tmp);
    fread(&tmp,sizeof(uint32_t),1,f_image);
    handle->image_magic = ntohl(tmp);
    
    //checks the 'magic number'
    if ((handle->label_magic != 2049) || (handle->image_magic != 2051))
    {
        free(handle);
        return MNIST_DATASET_INVALID;
    }

    //read number of images and number of items
    fread(&tmp,sizeof(uint32_t),1,f_label);  
    handle->number_item = ntohl(tmp);
    fread(&tmp,sizeof(uint32_t),1,f_image);
    handle->number_image = ntohl(tmp);   
    //check if number of images equals number of items
    if (handle->number_image != handle->number_item)
    {
        free(handle);
        return MNIST_DATASET_INVALID;
    }

    //read number of rows and number of columns
    fread(&tmp,sizeof(uint32_t),1,f_image);   
    handle->number_row = ntohl(tmp);
    fread(&tmp,sizeof(uint32_t),1,f_image);
    handle->number_col = ntohl(tmp);
    //calculate image size      
    int size = handle->number_row*handle->number_col;

    //malloc head
    handle->head = (mnist_image_handle)malloc(sizeof(struct mnist_image_t));
    handle->head->image = (unsigned char *)malloc(sizeof(unsigned char)*size);
    handle->head->previous = MNIST_IMAGE_INVALID;
    
    //read in first image and label
    fread(&handle->head->label,sizeof(uint8_t),1,f_label);
    uint8_t pixel;
    for(int i=0;i<size;i++){
        fread(&pixel,sizeof(uint8_t),1,f_image);
        handle->head->image[i] = pixel;
    }
	handle->head->next = MNIST_IMAGE_INVALID;
    //set prev to first image and current to the next 
    mnist_image_handle current = handle->head;
    mnist_image_handle prev = handle->head;

    //load all the image and label
    for(int i=1; i<handle->number_image; i++)
    {
		//create next
		current->next = (mnist_image_handle)malloc(sizeof(struct mnist_image_t));
		current->next->image = (unsigned char *)malloc(sizeof(unsigned char)*size);
		
		current = current->next;  //move to next
		current->previous = prev; //set previous

		fread(&current->label,sizeof(uint8_t),1,f_label);  //read current label
		//read in all pixels of the current image
		for(int j=0;j<size;j++){
		    fread(&pixel,sizeof(uint8_t),1,f_image);
		    current->image[j] = pixel;
		}
	        prev = current;   //move prev to the next
    }
		//last image have next to null
    		current->next=MNIST_IMAGE_INVALID;

    //close file
    fclose(f_label);
    fclose(f_image);

    return handle;
}

/// Same as mnist_open, except instead of loading all images from the dataset,
/// pick a random subset of k images from the dataset.
mnist_dataset_handle mnist_open_sample (const char * name, unsigned int k)
{
    //combine name with the rest part
    char * name_label = malloc(strlen(name) + 20);
	strcpy(name_label, name);
	strcat(name_label, "-labels-idx1-ubyte");
	
    char * name_image = malloc(strlen(name) + 20);
	strcpy(name_image, name);
	strcat(name_image, "-images-idx3-ubyte");

    //open file. if cannot open, return MNIST_DATASET_INVALID
    FILE * f_image;
    if((f_image = fopen(name_image,"rb"))==NULL)
    {
        free(name_image);
        free(name_label);
        return MNIST_DATASET_INVALID;
    }
    FILE * f_label;
    if((f_label = fopen(name_label,"rb"))==NULL)
    {
        free(name_image);
        free(name_label);
        return MNIST_DATASET_INVALID;
    }

    //free space for names
    free(name_image);
    free(name_label);

    //malloc space for handle
    mnist_dataset_handle handle = (mnist_dataset_handle)malloc(sizeof(struct mnist_dataset_t));

    //read the 'magic number'
    //fread(buffer,size,count,fp). read first 32 bits
    uint32_t tmp;
    fread(&tmp,sizeof(uint32_t),1,f_label);
    handle->label_magic = ntohl(tmp);
    fread(&tmp,sizeof(uint32_t),1,f_image);
    handle->image_magic = ntohl(tmp);
    
    //checks the 'magic number'
    if ((handle->label_magic != 2049) || (handle->image_magic != 2051))
    {
        free(handle);
        return MNIST_DATASET_INVALID;
    }

    //read number of images and number of items
    fread(&tmp,sizeof(uint32_t),1,f_label);
    handle->number_item = ntohl(tmp);
    fread(&tmp,sizeof(uint32_t),1,f_image);
    handle->number_image = ntohl(tmp);    
    //check if number of images equals number of items
    if (handle->number_image != handle->number_item)
    {
        free(handle);
        return MNIST_DATASET_INVALID;
    }

    int oldlength;
    //check if k is larger than the number of images
    if (handle->number_image < k)
    {
        free(handle);
        return MNIST_DATASET_INVALID;
    }
    else{
        oldlength = handle->number_image;
        handle->number_item = k;
        handle->number_image = k;
    }

    //read number of rows and number of columns
    fread(&tmp,sizeof(uint32_t),1,f_image);   
    handle->number_row = ntohl(tmp);
    fread(&tmp,sizeof(uint32_t),1,f_image);
    handle->number_col = ntohl(tmp);
    //calculate image size      
    int size = handle->number_row*handle->number_col;

    //malloc head
    handle->head = (mnist_image_handle)malloc(sizeof(struct mnist_image_t));
    handle->head->image = (unsigned char *)malloc(sizeof(unsigned char)*size);
    handle->head->previous = MNIST_IMAGE_INVALID;


    //get a random jump number. used in fseek
    srand(time(NULL));
    int jump = (rand()%(oldlength/k));
	if(oldlength==k || jump==0){jump=1;}

    int count=k;  //gather total k numbers
    int index=0;  //file read index starts at 0
    //make a jump on file start point
    count--; index+=jump;
    fseek(f_label,index*sizeof(uint8_t),0);
    fseek(f_image,index*sizeof(uint8_t)*size,0);    
    //read in first image and label
    fread(&handle->head->label,sizeof(uint8_t),1,f_label);
    uint8_t pixel;
    for(int i=0;i<size;i++){
        fread(&pixel,sizeof(uint8_t),1,f_image);
        handle->head->image[i] = pixel;
    }

	handle->head->next = MNIST_IMAGE_INVALID;
    //set prev to first image and current to the next 
    mnist_image_handle current = handle->head;
    mnist_image_handle prev = handle->head;

    //read the rest k-1 images
    while(count>0)
    {
        //jump to next random image
        count--; index+=jump;
        fseek(f_label,index*sizeof(uint8_t),0);
        fseek(f_image,index*sizeof(uint8_t)*size,0);

		//create next
		current->next = (mnist_image_handle)malloc(sizeof(struct mnist_image_t));
		current->next->image = (unsigned char *)malloc(sizeof(unsigned char)*size);
		
		current = current->next;  //move to next
		current->previous = prev; //set previous

		fread(&current->label,sizeof(uint8_t),1,f_label);  //read current label
		//read in all pixels of the current image
		for(int j=0;j<size;j++){
		    fread(&pixel,sizeof(uint8_t),1,f_image);
		    current->image[j] = pixel;
		}
	        prev = current;   //move prev to the next 
    }
	//last image have next to null
    current->next=MNIST_IMAGE_INVALID;	
    

	//close file
    fclose(f_label);
    fclose(f_image);

    return handle;
}



/// Create a new empty dataset
/// This only creates the in-memory representation of the dataset.
/// All images that will be added to the set will have size X and Y.
mnist_dataset_handle mnist_create(unsigned int x, unsigned int y){
    //malloc space for handle
    mnist_dataset_handle handle = (mnist_dataset_handle)malloc(sizeof(struct mnist_dataset_t));

    handle->number_row = x;
    handle->number_col = y;
    handle->image_magic = 2051;
    handle->label_magic = 2049; 
    handle->number_item = 0;
    handle->number_image = 0;
    handle->head = MNIST_IMAGE_INVALID;

    return handle;
}

/// Free the memory associated with the handle
void mnist_free (mnist_dataset_handle handle){
	//check if handle is valid        
	if(handle!=MNIST_DATASET_INVALID){
		mnist_image_handle current = handle->head;
		//free all the images one by one
		while(current != MNIST_IMAGE_INVALID){
			mnist_image_handle remove = current;
		    current=current->next;
		    free(remove->image);
		    free(remove);
        }
        //free data_set
        free(handle);
    }
}

/// Return the number of images in the dataset
/// Returns <0 if handle is equal to MNIST_DATASET_INVALID
int mnist_image_count (const_mnist_dataset_handle handle){
    if(handle==MNIST_DATASET_INVALID)
        return -1;
    else{
        return handle->number_image;
    }
}

/// Return the size of each image in the dataset; Stores the size in X and Y.
/// Sets *x and *y to 0 if handle == MNIST_DATASET_INVALID
void mnist_image_size (const_mnist_dataset_handle handle, unsigned int * x, unsigned int * y){
    if (handle==MNIST_DATASET_INVALID){
        *x=0;
        *y=0;
    }
    else{
        *x = handle->number_row;
        *y = handle->number_col;
    }
 
}


/// Return a handle to the first image in the dataset;
/// If there are no images in the dataset, or if handle ==
/// MNIST_DATASET_INVALID, or any other error occurs,
/// return MNIST_IMAGE_INVALID
mnist_image_handle mnist_image_begin (mnist_dataset_handle handle){
    if (handle==MNIST_DATASET_INVALID)
        return MNIST_IMAGE_INVALID;
    else if (handle->head == MNIST_IMAGE_INVALID)
        return MNIST_IMAGE_INVALID;
    else
        return handle->head;
}


/// Return a pointer to the data for the image.
const unsigned char * mnist_image_data (const_mnist_image_handle h){
	if (h == MNIST_IMAGE_INVALID)
		return NULL;
	else
		return h->image;
}


/// Obtain the label of the image.
/// Return <0 if the handle is invalid.
int mnist_image_label (const_mnist_image_handle h){
    if (h == MNIST_IMAGE_INVALID)
		return -1;
	else
		return ((int) h->label);
}



// Update the label for an image.
// Returns <0 if the handle is invalid (MNIST_IMAGE_INVALID)
// or >=0 if the lavel was changed successfully.
int mnist_set_image_label(mnist_image_handle h, unsigned int newlabel){
    if(h == MNIST_IMAGE_INVALID)
        return -1;
    else{
        h->label = (unsigned char)newlabel;
        return 0;
    }
}


/// Return the handle of the next image in the dataset, or MNIST_IMAGE_INVALID
/// if there are no more images.
mnist_image_handle mnist_image_next (mnist_image_handle h){
    if(h == MNIST_IMAGE_INVALID)
        return MNIST_IMAGE_INVALID;
    return h->next;
}

/// Return the handle of the previous image in the dataset, or
/// MNIST_IMAGE_INVALID if there is no previous image or the image handle was
/// invalid.
mnist_image_handle mnist_image_prev (mnist_image_handle h){
    if(h == MNIST_IMAGE_INVALID)
        return MNIST_IMAGE_INVALID;
    return h->previous;
}

/// Add a new image *after* the image represented by mnist_image_handle
/// Returns a handle referring to the new image
mnist_image_handle mnist_image_add_after (mnist_dataset_handle dataset, mnist_image_handle h, const unsigned char * imagedata, unsigned int x, unsigned int y){
    //error check
    if(x != dataset->number_row || y != dataset->number_col){
        return MNIST_IMAGE_INVALID;
    }
    
    //increment number
    dataset->number_item+=1;
    dataset->number_image+=1;
    
    //create image_handle need to be added
    int size = x*y;
    mnist_image_handle tmp = (mnist_image_handle) malloc(sizeof(struct mnist_image_t));
    tmp->image = (unsigned char *)malloc(sizeof(unsigned char)*size);
    memcpy(tmp->image, imagedata, size);

    //added as the first image
    if (h == MNIST_IMAGE_INVALID){
        tmp->next = dataset->head;
        dataset->head->previous = tmp;
        tmp->previous = MNIST_IMAGE_INVALID;
        dataset->head = tmp;
    }
    //added after h
    else{
        tmp->next = h->next;
        tmp->previous = h;
        h->next = tmp;
        tmp->next->previous = tmp;
    }

    return tmp;
}


/// Remove the image from the dataset;
/// Returns the handle to the image after the removed image.
/// Returns MNIST_IMAGE_INVALID if there is no next image or if
/// there dataset was MNIST_DATASET_INVALID or h was MNIST_IMAGE_INVALID
mnist_image_handle mnist_image_remove(mnist_dataset_handle dataset, mnist_image_handle h){
    //error check
    if(h == MNIST_IMAGE_INVALID || dataset ==  MNIST_DATASET_INVALID)
        return MNIST_IMAGE_INVALID;
    else{
        mnist_image_handle tmp = h->next; //return the next image

        //check if it is first image
        if(dataset->head == h){
            dataset->head = dataset->head->next;
            h->next->previous = MNIST_IMAGE_INVALID;
        }
        //if remove the last image
        else if(h->next == MNIST_IMAGE_INVALID){
            h->previous->next = MNIST_IMAGE_INVALID;
        }
        //if not remove the first or the last
        else{
            h->previous->next = h->next;
            h->next->previous = h->previous;
        }
        //free
        free(h->image);
        free(h);
        //decrement number
        dataset->number_item-=1;
        dataset->number_image-=1;

        //return
        return tmp;
    }
}

/// Persist the specified dataset to file.
bool mnist_save(const_mnist_dataset_handle h, const char * filename){
	if (h == MNIST_DATASET_INVALID)
		return false;

    //combine name with the rest part
    char * name_label = malloc(strlen(filename) + 20);
	strcpy(name_label, filename);
	strcat(name_label, "-labels-idx1-ubyte");
	
    char * name_image = malloc(strlen(filename) + 20);
	strcpy(name_image, filename);
	strcat(name_image, "-images-idx3-ubyte");


    //open file. if cannot open, return MNIST_DATASET_INVALID
    FILE * f_label;
    if((f_label = fopen(name_label,"wb"))==NULL)
    {
        //free space for names
        free(name_image);
        free(name_label);        
        return false;
    }
    FILE * f_image;
    if((f_image = fopen(name_image,"wb"))==NULL)
    {
        //free space for names
        free(name_image);
        free(name_label);        
        return false;
    }

    //free space for names
    free(name_image);
    free(name_label);


    //write magic number
    uint32_t tmp;
    tmp = htonl(h->label_magic);
    fwrite(&tmp, sizeof(uint32_t), 1, f_label);

    tmp = htonl(h->image_magic);
    fwrite(&tmp, sizeof(uint32_t), 1, f_image);


    //write number of images and items
    tmp = htonl(h->number_item);
    fwrite(&tmp, sizeof(uint32_t), 1, f_label);
    
    tmp = htonl(h->number_image);
    fwrite(&tmp, sizeof(uint32_t), 1, f_image);


    //write number of rows and columns
    tmp = htonl(h->number_row);
    fwrite(&tmp, sizeof(uint32_t), 1, f_image);
    
    tmp = htonl(h->number_col);
    fwrite(&tmp, sizeof(uint32_t), 1, f_image);    

    //write label and images
    int size = h->number_row*h->number_col;

    mnist_image_handle current = h->head;
    //copy every image one by one
    for(int i=0;i<h->number_image;i++){
        fwrite(&current->label, sizeof(uint8_t), 1, f_label);  //copy label

        //copy every image pixel
        for(int j=0; j<size;j++){
            fwrite((current->image+j), sizeof(uint8_t), 1, f_image);
        }

        current = current->next; //move to the next
    }

    //close files
    fclose(f_label);
    fclose(f_image);

    return true;

}



