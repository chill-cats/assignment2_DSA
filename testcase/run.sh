for ((i = 1; i <= 10000; i++)){
    python3 main.py > testcase
    ./mainQ testcase > out1
    ./mainK testcase > out2
    diff -w out1 out2 ||{
        touch error
        cp testcase error
        break
    }
    echo "Testcase$i: PASSED!"
}

echo "Success!" > result
shutdown -h
