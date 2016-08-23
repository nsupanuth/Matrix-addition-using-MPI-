//
//  main.c
//  Lab04
//  Supanuth Ongsuk
//  57070501049
//  Created by NUTH on 8/21/2559 BE.
//  Copyright © 2559 NUTHYLL. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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
    
    MPI_Request request[10];
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    
    if(rank==0){
        
        double starttime,endtime;
        
        int rows,columns;
        
        /* read matrix A */
        //fp1 = fopen("/Users/Nuth/Desktop/MatrixAddDataSet/matAlarge2.txt" , "r");
        fp1 = fopen("./matAlarge2.txt" , "r");
        fscanf(fp1,"%d %d",&rows,&columns);
        float **matA = alloc_2d_float(rows, columns);
        
        for (i=0; i<rows; i++) {
            for (j=0; j<columns; j++) {
                fscanf(fp1, "%f ",&matA[i][j]);
            }
        }
        
        fclose(fp1);
        
        /*read matrix B*/
        //fp2 = fopen("/Users/Nuth/Desktop/MatrixAddDataSet/matBlarge2.txt" , "r");
        fp2 = fopen("./matBlarge2.txt" , "r");
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
            MPI_Isend(&columns, 1, MPI_INT, i, 112, MPI_COMM_WORLD, &request[0]);
            MPI_Isend(&eachRow, 1, MPI_FLOAT, i, 111, MPI_COMM_WORLD,&request[1]);
            MPI_Isend(&matA[(eachRow*i)+ModRow][0], eachRow*columns, MPI_FLOAT, i, 113, MPI_COMM_WORLD,&request[2]);
            MPI_Isend(&matB[(eachRow*i)+ModRow][0], eachRow*columns, MPI_FLOAT, i, 114, MPI_COMM_WORLD,&request[3]);
            
        }
        
        for (i=0; i<eachRow+ModRow; i++) {
            for (j=0; j<columns; j++) {
                ans[i][j] = matA[i][j]+matB[i][j];
            }
        }

        
        for (i=1; i<size; i++) {
            MPI_Irecv(&ans[(eachRow*i)+ModRow][0],  eachRow*columns, MPI_FLOAT, i, 115, MPI_COMM_WORLD,&request[4]);
            MPI_Wait(&request[4], &status);
        }
        endtime = MPI_Wtime();
        
        //of = fopen("/Users/Nuth/Desktop/MatrixAddDataSet/solutionUnblock2.txt", "w");
        of = fopen("./solution.txt", "w");
        for (i=0; i<rows; i++) {
            for (j=0; j<columns; j++) {
                fprintf(of,"%.1f ",ans[i][j]);
            }
            fprintf(of,"\n");
        }
        
        fclose(of);
        
        printf("Total time : %lf\n",endtime-starttime);
        
    }else{
        
        int eachRow ;
        int column ;
        
        MPI_Irecv(&column, 1, MPI_INT, 0, 112, MPI_COMM_WORLD, &request[5]);
        MPI_Irecv(&eachRow, 1, MPI_INT, 0, 111, MPI_COMM_WORLD,&request[6]);
        
        MPI_Wait(&request[5], &status);
        MPI_Wait(&request[6], &status);
        
        float **matA = alloc_2d_float(eachRow,column);
        float **matB = alloc_2d_float(eachRow,column);
        float **ans = alloc_2d_float(eachRow,column);
        
        int i,j;
        
        MPI_Irecv(&matA[0][0],eachRow*column, MPI_FLOAT, 0, 113, MPI_COMM_WORLD,&request[7]);
        MPI_Irecv(&matB[0][0],eachRow*column, MPI_FLOAT, 0, 114, MPI_COMM_WORLD, &request[8]);
        
        MPI_Wait(&request[7], &status);
        MPI_Wait(&request[8], &status);
        
        for (i=0; i<eachRow; i++) {
            for (j=0; j<column; j++) {
                ans[i][j] = matA[i][j] + matB[i][j];
            }
        }
        
        MPI_Isend(&ans[0][0], eachRow*column, MPI_FLOAT, 0, 115, MPI_COMM_WORLD,&request[9]);
        MPI_Wait(&request[9], &status);

        
    }
    
    MPI_Finalize();
    
    
}