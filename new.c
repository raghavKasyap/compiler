#include stdio.h 
#include string.h

int main(){
    FILE* fptr = fopen("grammar.txt" , "r");
    
    if ( fptr == NULL )
    {
        printf( "grammar.txt file failed to open." ) ;
    }
    else{
        string arr[52];
        string str ="";
        int index=0, lastIndex=-1;
        while(fptr != NULL){
            fscanf(fptr,%s,str);
            if(index==0){
                arr[index] = str;
                index++;
                lastIndex++;
            }
            else{
                if(!strcmp(str,arr[lastIndex])){
                    
                }
                else{
                    arr[index]=str;
                    index++;
                    lastIndex++;
                }
            }
            while(*fptr != "\n"){
                fptr++;
            }
            fptr++;
            str="";

        }
        fclose(fptr);
        i=0;
        while(i<52){
            print(%s,arr[i]);
            i++;
        }
        return 0;
    }
}