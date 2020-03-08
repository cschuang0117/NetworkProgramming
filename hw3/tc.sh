#sudo tc qdisc add dev lo root netem loss 5% delay 10ms 3ms distribution normal rate 200kbit reorder 25%
sudo tc qdisc del dev lo root
#sudo tc qdisc show dev lo
#sudo tc qdisc add dev lo root netem rate 1Mbit