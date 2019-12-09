
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
   
int main(int argc, char *argv[]){
    int rank, size;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    int big_matrix[20][20];
    int matrix_2x2[2][3];
    int winner_2x2[2][3];
    int i; //variable that goes throught txt file
    int k = 0; //used to fill in big matrix row
    int j = 0; //used to fill in big matrix col
    int n = 0; // Used to see what dimensions the matrix is (since row == collumn)

    if(rank==0){
        FILE* file = fopen ("array.txt", "r");
        printf("Enter n : ");
        scanf("%d",&n);
        while (fscanf(file,"%d",&i)!=EOF) {   
            big_matrix[k][j] = i;
            j++;
            if(j>(n-1)){
                k++;
                j=0;
            }           
        }
        fclose (file);

        printf("Elements in the matrix are: \n");
        for(int i =0;i<n;i++){
             for(int j=0;j<n;j++){
                  printf("%d ",big_matrix[i][j]);
             }
             printf("\n");
        }

        int a = -1;
        int b = 0;
        for(int process = 1; process<size; process++){
             for (int i = 0; i<2; i++){
                a++;              
                if(a == n && n%2 == 0){
                    a = 0;
                    b = b + 2;
                }
                else if(a == (n-1) && n%2 == 1){
                    a = 0;
                    b = b + 2;
                }
                
                if(i == 0 && a!=n){
                    matrix_2x2[0][2] = a;
                }
                for(int j = 0; j<2; j++){
                    if(j == 0){
                        matrix_2x2[1][2] = b;
                    } 
                    matrix_2x2[i][j] = big_matrix[a][b];
                    b++;
                }
                b = b - 2;
             }
             printf("The 2x2 matrix for process %d is \n",process);
             for(int i = 0;i<2;i++){
                for(int j = 0;j<2;j++){
                    printf("%d ",matrix_2x2[i][j]);
                }
                printf("\n");
             }
             MPI_Send(&matrix_2x2, 6, MPI_INT, process, 0, MPI_COMM_WORLD);
        }
        printf("\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);

    int sum = 0;
    for(int i = 1; i<size; i++){
        if(rank == i){
            MPI_Recv(&matrix_2x2, 6, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i =0;i<2;i++){
                for(int j=0;j<2;j++){
                    sum+=matrix_2x2[i][j];
                }
             }
             printf("Process %d matrix sum is: %d\n",rank,sum);
        }
    }
   
    int one_process[2];
    int winning_process[2];
    one_process[0] = sum;
    one_process[1] = rank;
    MPI_Allreduce(one_process,winning_process,1,MPI_2INT,MPI_MAXLOC,MPI_COMM_WORLD);
    if(rank == winning_process[1]){
        for(int i = 0;i<2;i++){
            for(int j = 0;j<3;j++){
                winner_2x2[i][j] = matrix_2x2[i][j];
            }
        }
        MPI_Send(&winner_2x2,6, MPI_INT,0,0, MPI_COMM_WORLD);
    }

    if(rank==0){
        MPI_Recv(&winner_2x2,6, MPI_INT,winning_process[1],0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("\n");
        printf("Process with rank %d has highest 2x2 matrix sum which is %d\n",winning_process[1],winning_process[0]);
         printf("Elements in winning process %d are \n",winning_process[1]);
             for(int i = 0;i<2;i++){
                for(int j = 0;j<2;j++){
                    printf("%d ",winner_2x2[i][j]);
                }
                printf("\n");
             }
        printf("top left position of winning 2x2 matrix is :\n");
        printf("row: %d and col: %d ",winner_2x2[0][2],winner_2x2[1][2]);
    }
    MPI_Finalize();
    return 0;
}