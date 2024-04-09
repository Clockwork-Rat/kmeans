//kmeans_starter.c

#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



#define KMEANSITERS 10

//compile
//mpicc kmeans.c -lm -o kmeans

//run example with 2 means
//mpirun -np 4 -hostfile myhostfile.txt ./kmeans 5159737 2 2 iono_57min_5.16Mpts_2D.txt

//function prototypes
int importDataset(char * fname, int N, double ** dataset);

double edist(const double *centroid, const double *point, const int dim);

int main(int argc, char **argv) {

  int my_rank, nprocs;

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);


  //Process command-line arguments
  int N;
  int DIM;
  int KMEANS;
  char inputFname[500];


  if (argc != 5) {
    fprintf(stderr,"Please provide the following on the command line: N (number of lines in the file), dimensionality (number of coordinates per point/feature vector), K (number of means), dataset filename. Your input: %s\n",argv[0]);
    MPI_Finalize();
    exit(0);
  }

  sscanf(argv[1],"%d",&N);
  sscanf(argv[2],"%d",&DIM);
  sscanf(argv[3],"%d",&KMEANS);
  strcpy(inputFname,argv[4]);
  
  //pointer to entire dataset
  double ** dataset;

  

  if (N<1 || DIM <1 || KMEANS < 1)
  {
    printf("\nOne of the following are invalid: N, DIM, K(MEANS)\n");
    MPI_Finalize();
    exit(0);
  }
  //All ranks import dataset
  else
  {
   
    if (my_rank==0)
    {
    printf("\nNumber of lines (N): %d, Dimensionality: %d, KMEANS: %d, Filename: %s\n", N, DIM, KMEANS, inputFname);
    }



    //allocate memory for dataset
    dataset=(double**)malloc(sizeof(double*)*N);
    for (int i=0; i<N; i++)
    {
      dataset[i]=(double*)malloc(sizeof(double)*DIM);
    }

    int ret=importDataset(inputFname, N, dataset);

    if (ret==1)
    {
      MPI_Finalize();
      return 0;
    }    
  }

  MPI_Barrier(MPI_COMM_WORLD);

  
  //Write code here
  double stime = MPI_Wtime();
  double calc_time = 0.0;
  double sync_time = 0.0;
  // create assignments

  size_t all_ranges[nprocs][2];
  size_t local_ranges[2];
  
  // assign points
  if (my_rank == 0) {
    for ( int i = 0; i < nprocs; ++i ) {
      all_ranges[i][0] = i * N/nprocs;
      all_ranges[i][1] = (i + 1) * N/nprocs;

      // ensure last rank includes up to N
      if (i == nprocs - 1) {
        all_ranges[i][1] = N;
      }
    }
  }

  // scatter assigned points
  MPI_Scatter(all_ranges,
              2, MPI_UNSIGNED_LONG, 
              local_ranges, 2, 
              MPI_UNSIGNED_LONG, 0,
              MPI_COMM_WORLD);
  
  // assign means to be the first k points
  // initialize counts to 0
  double means[KMEANS][DIM];
  for (int i = 0; i < KMEANS; ++i) {
    for (int j = 0; j < DIM; ++j) {
      means[i][j] = dataset[i][j];
    }
  }
  
  // compare all locations
  double sums[KMEANS][DIM];
  size_t counts[KMEANS];

  for (int iteration = 0; iteration < 10; ++iteration) {
    // init counts and sums to zero
    for (size_t ctr = 0; ctr < KMEANS; ++ctr) {
      counts[ctr] = 0;
      for (size_t dim = 0; dim < DIM; ++dim) {
        sums[ctr][dim] = 0;
      }
    }
    double start_calc = MPI_Wtime();
    for (size_t i = local_ranges[0]; i < local_ranges[1]; ++i) {
      int closest = 0;
      double curr_distance = edist(means[0], dataset[i], DIM);
      for (size_t n = 1; n < KMEANS; ++n) {
        double tmp_distance = edist(means[n], dataset[i], DIM);
        if (tmp_distance < curr_distance) {
            closest = n;
            curr_distance = tmp_distance;
        }
      }
      for (int d = 0; d < DIM; ++d) {
        sums[closest][d] += dataset[i][d];
      }
      ++counts[closest];
    }
    double end_calc = MPI_Wtime();

    // print centroids
    if (my_rank == 0) {
        printf("k %d, iteration %d: ", KMEANS, iteration);
        for(int i = 0; i < KMEANS; ++i) {
          for(int d = 0; d < DIM - 1; ++d) {
            printf("%f, ", means[i][d]);
          }
          printf("%f\n", means[i][DIM - 1]);
        }
    }
    // allreduce
    // get total sum of counts
    size_t total_counts[KMEANS];
    double total_sums[KMEANS][DIM];

    double start_sync = MPI_Wtime();
    // all reduce to get total counts
    MPI_Allreduce(counts, total_counts, KMEANS, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);

    // get all means sums
    MPI_Allreduce(sums, total_sums, KMEANS * DIM, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    for (int n = 0; n < KMEANS; ++n) {
      if(total_counts[n] > 0) {
        // iterate through points and average them all this should be done at rank zero only for all points
        for (size_t i = 0; i < KMEANS; ++i) {
          for (size_t d = 0; d < DIM; ++d) {
            means[i][d] = total_sums[i][d] / total_counts[i];
          }
        }
      } else {
        // this means there are no closest point, so the centroid is reset to zero
        for (int i = 0; i < DIM; ++i) {
          means[n][i] = 0;
        }
      }
    }
    double end_sync = MPI_Wtime();

    sync_time += (end_sync - start_sync);
    calc_time += (end_calc - start_calc);
  }
  double max_calc;
  double max_sync;
  MPI_Reduce(&calc_time, &max_calc, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce(&sync_time, &max_sync, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

  double etime = MPI_Wtime();
  if (my_rank == 0) {
    printf("%d: Total time: %f\n", KMEANS, etime - stime);
    printf("%d: Calc time: %f\n",  KMEANS, max_calc);
    printf("%d: Sync time: %f\n",  KMEANS, max_sync);
  }

  //free dataset
  for (int i=0; i<N; i++)
  {
    free(dataset[i]);
  }

  free(dataset);
  MPI_Finalize();

  return 0;
}

double edist(const double *centroid, const double *point, const int dim) {
    double total_sq = 0.0;

    for (int i = 0; i < dim; ++i) {
        double curr = (centroid[i] - point[i]);
        total_sq += (curr * curr);
    }

    return sqrt(total_sq);
}

int importDataset(char * fname, int N, double ** dataset)
{

    FILE *fp = fopen(fname, "r");

    if (!fp) {
        printf("Unable to open file\n");
        return(1);
    }

    char buf[4096];
    int rowCnt = 0;
    int colCnt = 0;
    while (fgets(buf, 4096, fp) && rowCnt<N) {
        colCnt = 0;

        char *field = strtok(buf, ",");
        double tmp;
        sscanf(field,"%lf",&tmp);
        dataset[rowCnt][colCnt]=tmp;

        
        while (field) {
          colCnt++;
          field = strtok(NULL, ",");
          
          if (field!=NULL)
          {
          double tmp;
          sscanf(field,"%lf",&tmp);
          dataset[rowCnt][colCnt]=tmp;
          }   

        }
        rowCnt++;
    }

    fclose(fp);
    return 0;


}


