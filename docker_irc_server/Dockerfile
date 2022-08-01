from debian:11

run apt-get update && apt-get upgrade -y \
&& apt-get install -y openssl vim iproute2 tcpdump inspircd \
&& mkdir /var/run/inspircd

COPY ./irc_conf /etc/inspircd

expose 6667

cmd inspircd --runasroot ; tail -f
