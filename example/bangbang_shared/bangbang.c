

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>			//Needed for I2C port
#include <fcntl.h>			//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include "xmhfcrypto.h"
#include "picar-s.h"
#include "bangbang.h"
#include "bangbang_sharedlib.h"

__attribute__((section("i2c_section"))) unsigned char uhsign_key[]="super_secret_key_for_hmac";
#define UHSIGN_KEY_SIZE (sizeof(uhsign_key))



__attribute__((aligned(4096))) __attribute__((section("i2c_section_2")))   char encrypted_buffer[4096];
__attribute__((aligned(4096))) __attribute__((section("i2c_section_3")))  char decrypted_buffer[4096];
__attribute__((section(".palign_data")))  __attribute__((aligned(4096))) picar_s_param_t upicar;
__attribute__ ((section("i2c_section"))) static  int uobj_digital_list[NUM_REF]  = {0};
int uobj_references[NUM_REF] = {200,200,200,200,200};

int * uobj_read_analog(char *raw_result){
   int i,j;
   int high_byte, low_byte;
   static int analog_result[NUM_REF];
   if(raw_result != NULL){
      for(i=0;i<NUM_REF;i++){
         high_byte = raw_result[i*2] << 8;
         low_byte = raw_result[i*2+1];
         analog_result[i] = high_byte + low_byte;
      }
      return analog_result;
   }
   return NULL;
}


int * uobj_read_digital(char *buffer){
   int * lt;
   int i;
   lt = uobj_read_analog(buffer);
   if(lt != NULL){
      for(i=0;i<NUM_REF;i++){
         if(lt[i] > uobj_references[i]){
            uobj_digital_list[i] = 0;
         }
         else if(lt[i] < uobj_references[i]){
            uobj_digital_list[i] = 1;
         }
         else{
            uobj_digital_list[i] = -1;
         }
      }
   }
   //printf("read_digital() :: digital_list address : %p \n",uobj_digital_list);
  return uobj_digital_list;
}




/* Array used for retruning the results
   result_array[0] - speed
   result_array[1] - step
   result_array[2] - turn_angle
*/
__attribute__ ((section("i2c_section_4"))) static  int result_array[8]  = {0};

/* Private Functions */
void calculate_speed(int *array,int arr_len,int fw_speed,int *sp, int *st){
   const int a_step = 2;
   const int b_step = 8;
   const int c_step = 24;
   const int d_step = 40;
   if((array[0] == 0) && (array[1] == 0) && (array[2] == 1) &&
      (array[3] == 0) && (array[4] == 0) ){
      *st = 0;
   }
   else if(((array[0] == 0) && (array[1] == 1) && (array[2] == 1) &&
      (array[3] == 0) && (array[4] == 0) ) ||
      ((array[0] == 0) && (array[1] == 0) && (array[2] == 1) &&
      (array[3] == 1) && (array[4] == 0))) {
      *st =  a_step;
      *sp = fw_speed - 10;
   }
   else if(((array[0] == 0) && (array[1] == 1) && (array[2] == 0) &&
      (array[3] == 0) && (array[4] == 0) ) ||
      ((array[0] == 0) && (array[1] == 0) && (array[2] == 0) &&
      (array[3] == 1) && (array[4] == 0))) {
      *st = b_step;
      *sp = fw_speed - 15;
   }
   else if(((array[0] == 1) && (array[1] == 1) && (array[2] == 0) &&
      (array[3] == 0) && (array[4] == 0) ) ||
      ((array[0] == 0) && (array[1] == 0) && (array[2] == 0) &&
      (array[3] == 1) && (array[4] == 1))) {
      *st = c_step;
      *sp = fw_speed - 25;
   }
   else if(((array[0] == 1) && (array[1] == 0) && (array[2] == 0) &&
      (array[3] == 0) && (array[4] == 0) ) ||
      ((array[0] == 0) && (array[1] == 0) && (array[2] == 0) &&
      (array[3] == 0) && (array[4] == 1))) {
      *st = d_step;
      *sp = fw_speed - 35;
   }
   else{
      *st = d_step;
      *sp = fw_speed - 40;
   }
}


void calculate_angle(int *array,int arr_len,int *turn_angle, int st){
   if((array[0] == 0) && (array[1] == 0) && (array[2] == 1) &&
      (array[3] == 0) && (array[4] == 0) ){
      *turn_angle = 90;
   }
   else if(((array[0] == 0) && (array[1] == 1) && (array[2] == 1) &&
      (array[3] == 0) && (array[4] == 0) ) ||
      ((array[0] == 0) && (array[1] == 1) && (array[2] == 0) &&
      (array[3] == 0) && (array[4] == 0)) ||
      ((array[0] == 1) && (array[1] == 1) && (array[2] == 0) &&
      (array[3] == 0) && (array[4] == 0)) ||
      ((array[0] == 1) && (array[1] == 0) && (array[2] == 0) &&
      (array[3] == 0) && (array[4] == 0)) ) {
      *turn_angle = (int)(90 - st);
   }
   else if(((array[0] == 0) && (array[1] == 0) && (array[2] == 1) &&
      (array[3] == 1) && (array[4] == 0) ) ||
      ((array[0] == 0) && (array[1] == 0) && (array[2] == 0) &&
      (array[3] == 1) && (array[4] == 0)) ||
      ((array[0] == 0) && (array[1] == 0) && (array[2] == 0) &&
      (array[3] == 1) && (array[4] == 1)) ||
      ((array[0] == 0) && (array[1] == 0) && (array[2] == 0) &&
      (array[3] == 0) && (array[4] == 1)) ) {
      *turn_angle = (int)(90 + st);
   }
}


/* Public function */
int * calculate_angle_speed(char *buffer,int *array,int fw_speed,int turn_angle,int st){
   int speed = fw_speed;
   int step = st;
   int turning_angle = turn_angle;
   unsigned long digest_size = HMAC_DIGEST_SIZE;
   unsigned char digest_result[HMAC_DIGEST_SIZE];


   #ifdef UOBJCOLL
       picar_s_param_t *ptr_upicar = &upicar;
       int i;
       memcpy(encrypted_buffer,buffer,NUM_REF*2 + HMAC_DIGEST_SIZE);
       ptr_upicar->encrypted_buffer_va = (uint32_t) encrypted_buffer;
       ptr_upicar->decrypted_buffer_va = (uint32_t) decrypted_buffer;
       ptr_upicar->len = NUM_REF*2;
       // Perform an uobject call
             if(!uhcall(UAPP_PICAR_S_FUNCTION_TEST, ptr_upicar, sizeof(picar_s_param_t))){
                 //printf("hypercall FAILED\n");
             }
             else{
                 //printf("hypercall SUCCESS\n");
          memcpy(digest_result,decrypted_buffer,digest_size);
          digest_size = HMAC_DIGEST_SIZE;
             }

             // Sleep for 10ms so that an attack can succeed in overwriting bytes in buffer
             // Car still runs fine with this delay
             usleep(10000);
       if(memcmp(encrypted_buffer+NUM_REF*2,decrypted_buffer,digest_size) != 0){
                   printf("HMAC digest did not match with driver's digest \n");
                   //printf("Bytes returned: ");
                   for(i=0;i<NUM_REF*2+HMAC_DIGEST_SIZE;i++){
                      //printf("%X ",encrypted_buffer[i]);
                   }
                   //printf("\nDigest calculated: ");
                   for(i=0;i<HMAC_DIGEST_SIZE;i++){
                      //printf("%X ",digest_result[i]);
                   }
                   for(i=0;i<NUM_REF*2;i++){
                        buffer[i] = 0;
                   }
                   //printf("\n");
             }
             else{
                //printf("Digest match\n");
             }
   #else
             // Calculate the HMAC
             if(hmac_sha256_memory(uhsign_key, (unsigned long) UHSIGN_KEY_SIZE, (unsigned char *) buffer, (unsigned long) length, digest_result, &digest_size)==CRYPT_OK) {
                  if(memcmp(buffer+length,digest_result,digest_size) != 0){
                      //printf("HMAC digest did not match with driver's digest \n");
                      printf("Bytes returned: ");
                      for(i=0;i<length;i++){
                        //printf("%d ",buffer[i]);
                      }
                      //printf("\nDigest calculated: ");
                      for(i=0;i<HMAC_DIGEST_SIZE;i++){
                         //printf("%d ",digest_result[i]);
                      }
                      for(i=0;i<length;i++){
                         buffer[i] = 0;
                      }
                      //printf("\n");
                  }
                 // else{
                 //    printf("HMAC digest match\n");
                 // }
             }
   #endif
   calculate_speed(array,NUM_REF,fw_speed,&speed,&step);
   calculate_angle(array,NUM_REF,&turning_angle,step);
   /* Return the other three parameters to the caller (Python) */
   result_array[0] = speed;
   result_array[1] = step;
   result_array[2] = turning_angle;
   return result_array;
}
