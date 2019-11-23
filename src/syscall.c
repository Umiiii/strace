#include "sysent.h"
#include "errnoent.h"
#include <stdio.h>
static const char* err_name(int err){
    return errnoent[err];
}

int main(){
    printf("%s",err_name(1));
}