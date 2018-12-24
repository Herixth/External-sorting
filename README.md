# External-sorting
DSAA works

### record
- key: float -4bytes
- val: unsigned int -4bytes

### block
- 4096 bytes
- 512 records

### random_creater
created random records for 8N blocks
```shell
./random_creater.exe
>> ...filename: REC.dat
>> ...N: 10
```
### extsort
```shell
./extsort.exe REC.dat STAT.dat
```
get a copy of REC.dat(cp_REC.dat), and REC.dat is sorted.

### check_sorted
```shell
./check_sorted.exe REC.dat
```
to check whether REC.dat is sorted.
