rm -f /tmp/.start

/etc/init.d/mme stop
sleep 1
/etc/init.d/ims stop
sleep 1
/etc/init.d/enb stop
