clear
echo "-----info-----"
recFile="REC.dat"
read -p "input statfile: " statFile
read -p "input minN: " minN
read -p "input maxN: " maxN
read -p "input test round: " divide
srcFile="src.tmp"
echo "-----init-----"
cprecFile="cp_"${recFile}
clear
echo "-----test-----"
for ((N=$minN; N<=$maxN; N++))
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

