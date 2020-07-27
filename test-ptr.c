#include <stdio.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    u_int16_t array[10] = {1,2,3,4,5,6,7,8,9,0};
    u_int32_t *ptr,*ptr2,*ptr3;

    ptr=(u_int32_t *)&array;
    ptr2=ptr+1;
    ptr3=ptr+2;


    #ifdef PRINT
        printf("%lu %lu %lu\n",*ptr, *ptr2, *ptr3);
    #endif

    #ifdef WRITE
        FILE *out;
        out=fopen("output","wb+");
        fwrite(&array,2,10,out);
        fwrite((ptr),4,3,out);
        fwrite((ptr2),4,3,out);
        fwrite((ptr3),4,3,out);
        fclose(out);
    #endif

    return(0);
}