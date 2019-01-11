clear
echo "-----info-----"
recFile="REC.dat"
read -p "input statfile: " statFile
read -p "input minN: " minN
read -p "input maxN: " maxN
read -p "input step: " step
read -p "input test round: " divide
srcFile="src.tmp"
echo "-----init-----"
cprecFile="cp_"${recFile}
if [ $minN -eq 1 ]; then
    rm -f $statFile
    echo "N,file size,records num,total runs,runs-var,run-build(s),run-build-var,run-merge(s),run-merge-var,total cost(s),total-var,state" > $statFile
fi
clear
echo "-----test-----"
for ((N=$minN; N<=$maxN; N+=$step))
do
    rm -f $srcFile $recFile $cprecFile
    for ((round=1; round<=divide; round++))
    do
        echo "N: $N: round: $round"
        ./random_creater.exe $recFile $N
        ./extsort.exe $recFile $srcFile
        ./check_sorted.exe $recFile $srcFile
    done
    ./unit.exe $srcFile $statFile
    rm -f $srcFile
done
rm -f $cprecFile $recFile

