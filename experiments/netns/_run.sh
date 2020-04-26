echo $$ > $1
while :; do
	sleep 3
    echo "----------\n$(ip addr)" > $2
done
