if diff RFresult.txt RFresult_answer.txt; then
        printf "AC\n"
    else
        printf "Wa\n"
        exit 0
    fi
if diff dmemresult_answer.txt dmemresult.txt; then
        printf "AC\n"
    else
        printf "Wa\n"
        exit 0
    fi