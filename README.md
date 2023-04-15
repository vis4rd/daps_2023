## Distributed and Parallel Systems

MPI programs written on PG's labs.

Semester 8, year 2023.

### Setup on AGH local machine

```sh
ssh -X stud204-06
source /opt/nfs/config/source_mpich401.sh
which mpiexec #should list /opt/nfs directory
/opt/nfs/config/station204_name_list.sh 1 16
/opt/nfs/config/station204_back_test.sh 1 16

/opt/nfs/config/station204_name_list.sh 1 16 > nodes

uname -n # local
mpiexec -f nodes -n 16 uname -n # distributed

cp nodes nodes1 # create file with only working nodes
# remove not working nodes from mpiexec command
mpiexec -f nodes1 -n 16 uname -n
```
