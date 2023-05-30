#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

# define max_line 100 
# define max_resource 100 
# define max_row 100 

void read_resources(char file_name [],int * resource_matric,int* size){
    // function to read the resource txt file ; 
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
// function to fetch the request and allocation matrices ; 
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
void free_2d(int (**array_2d), int row_size){
    for(int i=0;i<row_size;i++){
        free(array_2d[i]) ;
    }
    free(array_2d); 
}

struct matrix{
    int (*resource_matrix) ; 
    int (**request_matrix) ;
    int (**allocation_matrix);
    int (**need_matrix) ; 
    int (*is_waiting) ; 
};
// allocate memory for matrix store without spesific content 
void initialize_matrix_struct(struct matrix* matrix_store, int size_rows, int size_cols) {

    // allocate memory for resource matrix
    matrix_store->resource_matrix = (int*)malloc(size_cols * sizeof(int));
    // Allocate memory for 2D matrices
    matrix_store->request_matrix = (int**)malloc(size_rows * sizeof(int*));
    matrix_store->allocation_matrix = (int**)malloc(size_rows * sizeof(int*));
    matrix_store->need_matrix = (int**)malloc(size_rows * sizeof(int*));
    matrix_store->is_waiting= (int*)malloc(size_rows*sizeof(int)); 
    for (int i = 0; i < size_rows; i++) {
        matrix_store->request_matrix[i] = (int*)malloc(size_cols* sizeof(int));
        matrix_store->allocation_matrix[i] = (int*)malloc(size_cols * sizeof(int));
        matrix_store->need_matrix[i] = (int*)malloc(size_cols * sizeof(int));
    }
}
// deallocate memory for the matrix
void free_matrix(struct matrix matrix_store,int size){
    free(matrix_store.resource_matrix);
    for (int i = 0; i < size; i++) {
        free(matrix_store.request_matrix[i]);
        free(matrix_store.allocation_matrix[i]);
        free(matrix_store.need_matrix[i]); 
    }
    free(matrix_store.is_waiting);
    free(matrix_store.request_matrix);
    free(matrix_store.allocation_matrix);
    free(matrix_store.need_matrix);
}
// fill the matrix store with the content fetched from given txt files 
void fill_matrix(  
    struct matrix* matrix_store,  
    int (*resource_matrix),
    int (**request_matrix),
    int (**allocation_matrix),
    int row_size,
    int col_size)
{
    for(int t=0;t<col_size;t++){
        matrix_store->resource_matrix[t]= resource_matrix[t];
    }
    for(int i=0; i<row_size;i++){
        matrix_store->is_waiting[i]=1; 
        for(int j=0;j<col_size;j++){
            matrix_store->request_matrix[i][j]= request_matrix[i][j];
            matrix_store->allocation_matrix[i][j]= allocation_matrix[i][j];
            matrix_store->need_matrix[i][j]= (request_matrix[i][j]- allocation_matrix[i][j]);
        }
    }
}
// function to print out according to the first task in the description
void print_matrix_store (struct matrix* matrix_store,int size_row,int size_col){
    for(int i=0; i<size_row;i++){
        printf("Information for Process : P%d:\n",(i+1)); 
        printf("Allocated Resources: ");
        for(int j=0;j<size_col;j++){
            printf("R%d:",(j+1));
            printf("%d ",matrix_store->allocation_matrix[i][j]);
        }
        printf("\n");
        printf("Resource Request: ");
        for(int j=0;j<size_col;j++){
            printf("R%d:",(j+1));
            printf("%d ",matrix_store->request_matrix[i][j]);
        }
        printf("\n");
        printf("Need Matrix: "); 
        for(int j=0;j<size_col;j++){
            printf("R%d:",(j+1));
            printf("%d ",matrix_store->need_matrix[i][j]);
        }
        printf("\n");
    }
}

// in each iteration, if the idx process can be implemented, deallocate its resource to the system
// to collect its sources, update the resource matrix ;
void update_resource_matrix(struct matrix* matrix_store,int num_resource,int index){
    for(int i=0 ;i<num_resource;i++){
        matrix_store->resource_matrix[i]+=matrix_store->allocation_matrix[index][i];
        matrix_store->allocation_matrix[index][i]=0;  
    }
    matrix_store->is_waiting[index]= 0 ; 
}

// function to print out the path; 
void print_path(int* path, int size){
    for(int i=0 ;i<size;i++){
        printf("P%d ",path[i]);
    }
    printf("\n"); 
}


int check_unsafe_state(struct matrix* matrix_store, int num_processes,int num_resource){
    int *path= (int*)malloc(num_processes * sizeof(int));
    int path_counter=0 ; 
    int checker= 1 ;
    int i=0 ;
    int check_counter=0 ; // increment by one whenever a process is found to execute, and break the loop when the counter==total processes
    int infinite_loop_checker=0 ; // check infinite loop 
    int first_index=1 ; 
    int loop_counter=0  ; 

    while(1){
        if( ((i==infinite_loop_checker) || (check_counter==num_processes)) && (first_index==0)){
            // again the same index without finding any process, or all the processes executed
            break ;
        }

        if(matrix_store->is_waiting[i]==0){
            i+=1 ;
            i= i%num_processes; 
            continue;
        }
        int checker_2= 1; 
        for(int j=0;j<num_resource;j++){
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
            first_index=0 ; 
            path[path_counter] = i ;
            path_counter++;  
            update_resource_matrix(matrix_store,num_resource,i);
            infinite_loop_checker= i ; // the index of the last process ;
            check_counter++;
        }
            i++ ;
            i= (i%num_processes); 
        
    }
    // iterate over the matrix store is waiting array and check the states of the processses
    // if there is a waiting process return false; 
    for(int i=0;i<num_processes;i++){
        if(matrix_store->is_waiting[i]==0){
            continue;
        }
        else{
            return 0 ; 
        }
    }
    printf("Running ORDER for Processes: "); 
    print_path(path,num_processes); 
    return 1 ; 
}

void run_banker(struct matrix* matrix_store, int num_processes, int num_resource){
    if (check_unsafe_state(matrix_store,num_processes,num_resource)==1){
        return ; 
    }

    printf("System is an Unsafe State. ");
    int counter=0 ; 
    for(int i=0;i<num_processes;i++){
        if(matrix_store->is_waiting[i]==1){
            if(counter==0){
                printf("P%d",(i+1)); 
                counter=1; 
            }
            else {
                printf(" and P%d",(i+1));
            }
        }
    }
    printf(" is/are the cause of the deadlock ! "); 
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
    // we have extra-allocated memory for the allocation matrix; 
    // deallocate the extra space : 
    for (int i=(*size_process);i<max_row;i++){
        free(allocation_matrix[i]); 
    }

    // alllocate memory for rows of the request matrix; 
    int** request_matrix= (int**)malloc(*size_process * sizeof(int*));
    // allocate memory for the columns; 
    for (int i = 0; i < *size_process; i++) {
        request_matrix[i] = (int*)malloc(*size * sizeof(int));
    }
    create_matrix("requests.txt",request_matrix,size_process,*size); 

    // allocate memory for rows of the need matrix; 
    int ** need_matrix=(int**)malloc(*size_process*sizeof(int*)); 
    // allocate memory for the columns ; 
    for (int i = 0; i < *size; i++) {
        need_matrix[i] = (int*)malloc(*size * sizeof(int));
    }
    // calculate need matrix ;
    for(int i=0;i<*size;i++){
        for(int j=0;j<*size;j++){
            need_matrix[i][j]= request_matrix[i][j]-allocation_matrix[i][j];
        }
    }

    // initialize and fill matrix store struct that contains all the matrices ;
    struct matrix matrix_store ; 
    initialize_matrix_struct(&matrix_store,*size_process,*size); 
    fill_matrix(&matrix_store,resource_matrix,request_matrix,allocation_matrix,*size_process,*size);     
    free_2d(allocation_matrix,*size_process);
    free_2d(request_matrix,*size_process);
    print_matrix_store(&matrix_store,*size_process,*size) ; 
    printf("\n\n\n\n"); 
    run_banker(&matrix_store,*size_process,*size);
    free_matrix(matrix_store,*size_process);
    return 0;
}
