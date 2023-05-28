#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

# define max_line 100 
# define max_resource 100 
# define max_row 100 

void read_resources(char file_name [],int * resource_matric,int* size){
    FILE* filep; 
    filep= fopen(file_name,"r"); 
    char line[max_line];
    char c; 
    int counter=0 ;
    int i=0 ; 
    if(filep==NULL){
        return ;
    }
    while (1) {
        char s[5];
        while ((c = fgetc(filep)) != ' ' && c != EOF) {
            s[i] = c;
            i++;
        }

        s[i] = '\0'; 
        resource_matric[counter]=atoi(s);
        i = 0;
        counter++;
        if (c == EOF) {
            break;
        }
    }
    *size= counter; 
    fclose(filep);
}


void create_matrix(char file_name[], int(**matrix),int* num_processes,int col_size){
    // col_size -> number of resources ;
    // row_size -> num_processes ; 
    FILE *filep ; 
    filep= fopen(file_name,"r");
    char line [max_line];
    char c; 
    int i=0;
    int row_counter=0 ; 
    int col_counter=0 ; 
    if(filep==NULL){
        return;
    }
    while (fgets(line, max_line, filep)) {
        i=0 ;     
        char num[5];
        int a = 0;
        int i = 0;
        col_counter= 0;
        while (line[i-1] != '\0') {
            if (line[i] != ' ') {
                num[a]=line[i];
                i++;
                a++;
            } 
            if (line[i] == ' ' || ((col_counter+1) == col_size)) {
                //printf("row counter: %d , last column: %d \n",row_counter,((col_counter+1)==col_size));
                num[a] = '\0';
                matrix[row_counter][col_counter]=atoi(num);
                a = 0;
                col_counter++;
                i++;
            }
        }
    row_counter++ ;        
    }
    *num_processes=row_counter;
    fclose(filep);

}
void print_2d_matrix(int** matrix,int row_counter,int col_counter){
    for(int i=0;i<row_counter;i++){
        for(int j=0;j<col_counter;j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}
void print(int **request_matrix, int** allocation_matrix,int row_size,int col_size ){
    // print the necessary info for the corresponding process
    int num_processes= row_size ; 
    for(int i=0;i<num_processes;i++){
        printf("Information for Process : P%d:\n",(i+1)); 
        printf("Allocated Resources: ");
        for(int j=0;j<col_size;j++){
            printf("R%d:",(j+1));
            printf("%d ",allocation_matrix[i][j]);
        }
        printf("\n"); 
        printf("Resource Request : "); 
        for(int j=0;j<col_size;j++){
            printf("R%d:",(j+1));
            printf("%d ",request_matrix[i][j]);

        }
        printf("\n");
    }
}


struct matrix{
    int (*resource_matrix) ; 
    int (**request_matrix) ;
    int (**allocation_matrix);
    int (**need_matrix) ; 
};
void initialize_matrix_struct(struct matrix* matrix_store, int size_rows, int size_cols) {
    matrix_store->resource_matrix = (int*)malloc(size_cols * sizeof(int));

    // Allocate memory for 2D matrices
    matrix_store->request_matrix = (int**)malloc(size_rows * sizeof(int*));
    matrix_store->allocation_matrix = (int**)malloc(size_rows * sizeof(int*));
    matrix_store->need_matrix = (int**)malloc(size_rows * sizeof(int*));
    for (int i = 0; i < size_rows; i++) {
        matrix_store->request_matrix[i] = (int*)malloc(size_cols* sizeof(int));
        matrix_store->allocation_matrix[i] = (int*)malloc(size_cols * sizeof(int));
        matrix_store->need_matrix[i] = (int*)malloc(size_cols * sizeof(int));
    }
}
void free_matrix(struct matrix matrix_store,int size){
    free(matrix_store.resource_matrix);
    for (int i = 0; i < size; i++) {
        free(matrix_store.request_matrix[i]);
        free(matrix_store.allocation_matrix[i]);
        free(matrix_store.need_matrix[i]); 
    }
    free(matrix_store.request_matrix);
    free(matrix_store.allocation_matrix);
    free(matrix_store.need_matrix);
}
void fill_matrix(  
    struct matrix* matrix_store,  
    int (*resource_matrix),
    int (**request_matrix),
    int (**allocation_matrix),
    int size)
{
    for(int i=0; i<size;i++){
        matrix_store->resource_matrix[i]= resource_matrix[i];
        for(int j=0;j<size;j++){
            matrix_store->request_matrix[i][j]= request_matrix[i][j];
            matrix_store->allocation_matrix[i][j]= allocation_matrix[i][j];
            matrix_store->need_matrix[i][j]= (request_matrix[i][j]- allocation_matrix[i][j]);
        }
    }
}
void print_matrix_store (struct matrix* matrix_store,int size){
    for(int i=0; i<size;i++){
        printf("Information for Process : P%d:\n",(i+1)); 
        printf("Allocated Resources: ");
        for(int j=0;j<size;j++){
            printf("R%d:",(j+1));
            printf("%d ",matrix_store->allocation_matrix[i][j]);
        }
        printf("\n");
        printf("Resource Request: ");
        for(int j=0;j<size;j++){
            printf("R%d:",(j+1));
            printf("%d ",matrix_store->request_matrix[i][j]);
        }
        printf("\n");
        printf("Need Matrix: "); 
        for(int j=0;j<size;j++){
            printf("R%d:",(j+1));
            printf("%d ",matrix_store->need_matrix[i][j]);
        }
        printf("\n");
    }
}

int check_unsafe_state(struct matrix* matrix_store, int size){
    int checker= 1 ; 
    for(int i=0 ;i<size;i++){
        int checker_2= 1; 
        for(int j=0;j<size;j++){
            if(checker_2==0){
                checker=0 ;
                break;
            }
            if(matrix_store->need_matrix[i][j]<= matrix_store->resource_matrix[j]){
                checker=1 ;
                checker_2=1 ;
            }
            else{
                checker_2=0 ; 
            }
        }
        if(checker==1){
            return 1 ;
        }
    }
    return 0 ; 
}
int main() {
    int resource_matrix [max_resource]; 
    int* size = malloc(1*sizeof(int)); // size of the resources 
    int* size_process = malloc(1*sizeof(int));  // size of the processes 
    read_resources("resources.txt",resource_matrix,size);
    // allocate memory for rows 
    int** allocation_matrix= (int**)malloc(max_row * sizeof(int*));
    // allocate memory for columns 
    for (int i = 0; i <max_row; i++) {
        allocation_matrix[i] = (int*)malloc(*size * sizeof(int));
    }
    create_matrix("allocations.txt",allocation_matrix,size_process,*size) ;
    int** request_matrix= (int**)malloc(*size_process * sizeof(int*));
    // allocate memory 
    for (int i = 0; i < *size_process; i++) {
        request_matrix[i] = (int*)malloc(*size * sizeof(int));
    }
    create_matrix("requests.txt",request_matrix,size_process,*size); 
    //print_2d_matrix(request_matrix,*size,*size) ;
    int ** need_matrix=(int**)malloc(*size*sizeof(int*)); 
    for (int i = 0; i < *size; i++) {
        need_matrix[i] = (int*)malloc(*size * sizeof(int));
    }
    for(int i=0;i<*size;i++){
        for(int j=0;j<*size;j++){
            need_matrix[i][j]= request_matrix[i][j]-allocation_matrix[i][j];
        }
    }

    print(request_matrix,allocation_matrix,*size,*size);
    printf("---------------------------------------------------------\n");

    // initialize and fill matrix store struct that contains all the matrices ;
    struct matrix matrix_store ; 
    initialize_matrix_struct(&matrix_store,*size_process,*size); 
    fill_matrix(&matrix_store,resource_matrix,request_matrix,allocation_matrix,*size); 
    print_matrix_store(&matrix_store,*size) ; 
    
    return 0;
}
