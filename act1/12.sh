#!/bin/bash
#SBATCH --job-name=kmeans-1-4
#SBATCH --output=/home/ijr44/cs552/a5/act1/run4.txt
#SBATCH --error=/home/ijr44/cs552/a5/act1/run4.err
#SBATCH --time=100:00
#SBATCH --mem=100G
#SBATCH --nodes=1
#SBATCH --ntasks=12
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C bw

module load openmpi

mpicc -O3 ~/cs552/kmeans/kmeans_act1_raspet.c -lm -o ~/out_act1_4

srun ~/out_act1_4 5159737 2 2 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 25 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 50 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 100 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 2 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 25 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 50 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 100 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 2 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 25 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 50 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_4 5159737 2 100 ~/iono_57min_5.16Mpts_2D.txt