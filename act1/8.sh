#!/bin/bash
#SBATCH --job-name=kmeans-1-3
#SBATCH --output=/home/ijr44/cs552/a5/act1/run3.txt
#SBATCH --error=/home/ijr44/cs552/a5/act1/run3.err
#SBATCH --time=10:00
#SBATCH --mem=20G
#SBATCH --nodes=1
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=1
#SBATCH --exclusive
#SBATCH -C bw

module load openmpi

mpicc -O3 ~/cs552/kmeans/kmeans_act1_raspet.c -lm -o ~/out_act1_3

srun ~/out_act1_3 5159737 2 2 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 25 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 50 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 100 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 2 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 25 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 50 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 100 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 2 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 25 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 50 ~/iono_57min_5.16Mpts_2D.txt

srun ~/out_act1_3 5159737 2 100 ~/iono_57min_5.16Mpts_2D.txt