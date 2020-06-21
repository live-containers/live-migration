#!/bin/bash
awk 'NF && $1!~/^#/' key_scalability.dat | \
awk 'BEGIN { FS="," }
    {sum_cp += $1;
      sum_rs += $2;
      sum_tot += $3;
      sum_tot_2 += $3 * $3
    } (NR%10) == 0 {
        avg = sum_tot/10;
        print sum_cp/10, sum_rs/10, avg, sum_tot_2/10 - avg * avg;
        sum_cp = 0;
        sum_rs = 0;
        sum_tot = 0;
        sum_tot_2 = 0;
    }' 
