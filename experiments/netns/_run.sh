echo $$ > $1

echo "Going to sleep." > $2
sleep 4
while :; do
	test -f ip${3} && break;
	sleep 0.2
done
IP=$(< ip${3})
echo "Got the IP: ${IP}" >> $2

nc -l -s ${IP} -p 9999 >> $2 < /dev/zero
while :; do
	sleep 3
done
echo "Disconnected!" >> $2
