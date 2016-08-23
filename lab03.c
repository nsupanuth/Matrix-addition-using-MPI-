//
//  main.c
//  Lab03
//  Supanuth Ongsuk
//  57070501049
//  Created by NUTH on 8/17/2559 BE.
//  Copyright © 2559 NUTHYLL. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define tag 1

FILE *fp1,*fp2,*of;


float **alloc_2d_float(int rows,int cols){
    int i ;
    float *data = (float *)malloc(rows*cols*sizeof(float));
    float **array = (float **)malloc(rows*sizeof(float*));
    for (i=0; i<rows; i++) {
        array[i] = &(data[cols*i]);
    }
    return array;
}


int main(int argc,char * argv[]) {
    
    int rank,size;
    int i,j;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    if(rank==0){
        double starttime,endtime;
        int rows,columns;
        
        /* read matrix A */
        fp1 = fopen("/Users/Nuth/Desktop/MatrixAddDataSet/matAlarge2.txt" , "r");
        //fp1 = fopen("./matAlarge2.txt" , "r");
        fscanf(fp1,"%d %d",&rows,&columns);
        float **matA = alloc_2d_float(rows, columns);
        
        for (i=0; i<rows; i++) {
            for (j=0; j<columns; j++) {
                fscanf(fp1, "%f ",&matA[i][j]);
            }
        }
        
        fclose(fp1);
        
        /*read matrix B*/
        fp2 = fopen("/Users/Nuth/Desktop/MatrixAddDataSet/matBlarge2.txt" , "r");
        //fp2 = fopen("./matBlatge2.txt" , "r");

        fscanf(fp2,"%d %d",&rows,&columns);
        float **matB = alloc_2d_float(rows, columns);
        
        for (i=0; i<rows; i++) {
            for (j=0; j<columns; j++) {
                fscanf(fp2, "%f ",&matB[i][j]);
            }
        }
        
        fclose(fp2);
        
        starttime = MPI_Wtime();
        /* add in part of rank 0 */
        int eachRow = rows/size;
        int ModRow = rows%size;
        float **ans = alloc_2d_float(rows, columns);
        
        
        for (i=1; i<size; i++) {
            MPI_Send(&columns, 1, MPI_INT, i, 112, MPI_COMM_WORLD);
            MPI_Send(&eachRow, 1, MPI_FLOAT, i, 111, MPI_COMM_WORLD);
            MPI_Send(&matA[(eachRow*i)+ModRow][0], eachRow*columns, MPI_FLOAT, i, 113, MPI_COMM_WORLD);
            MPI_Send(&matB[(eachRow*i)+ModRow][0], eachRow*columns, MPI_FLOAT, i, 114, MPI_COMM_WORLD);
        }
        
        for (i=0; i<eachRow+ModRow; i++) {
            for (j=0; j<columns; j++) {
                ans[i][j] = matA[i][j]+matB[i][j];
            }
        }

        
        for (i=1; i<size; i++) {
            MPI_Recv(&ans[(eachRow*i)+ModRow][0], eachRow*columns, MPI_FLOAT,i, 115, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        
        endtime = MPI_Wtime();
        
        of = fopen("/Users/Nuth/Desktop/MatrixAddDataSet/solution.txt", "w");
        //of = fopen("./solution.txt", "w");
        fprintf(of,"%d %d \n",rows,columns);
        for (i=0; i<rows; i++) {
            for (j=0; j<columns; j++) {
                fprintf(of,"%.1f ",ans[i][j]);
            }
            fprintf(of,"\n");
        }
        
        fclose(of);
        printf("Total %lf sec.\n",endtime-starttime);

        
    }else{
        
        int eachRow ;
        int column ;
        
        MPI_Recv(&column, 1, MPI_INT, 0, 112, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&eachRow, 1, MPI_INT, 0, 111, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        
        float **matA = alloc_2d_float(eachRow,column);
        float **matB = alloc_2d_float(eachRow,column);
        float **ans = alloc_2d_float(eachRow,column);
        
        int i,j;

        MPI_Recv(&matA[0][0],eachRow*column, MPI_FLOAT, 0, 113, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Recv(&matB[0][0],eachRow*column, MPI_FLOAT, 0, 114, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        for (i=0; i<eachRow; i++) {
            for (j=0; j<column; j++) {
                ans[i][j] = matA[i][j] + matB[i][j];
            }
        }
        
        MPI_Send(&ans[0][0], eachRow*column, MPI_FLOAT, 0, 115, MPI_COMM_WORLD);
        
    }
    
    MPI_Finalize();
    
    
}