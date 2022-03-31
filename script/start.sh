touch /tmp/.start

/root/Amarisoft/mme/lte_init.sh
/etc/init.d/mme start
sleep 1
/root/Amarisoft/mme/lte_init.sh
/etc/init.d/ims start
sleep 1
/root/Amarisoft/enb/lte_init.sh

rm -f /root/Amarisoft/enb/config/enb.cfg

case "$1" in
	sk_lte)
		ln -s /root/Amarisoft/enb/config/enb_sk_lte.cfg /root/Amarisoft/enb/config/enb.cfg
		;;
	kt_lte)
		ln -s /root/Amarisoft/enb/config/enb_kt_lte.cfg /root/Amarisoft/enb/config/enb.cfg
		;;
	lg_lte)
		ln -s /root/Amarisoft/enb/config/enb_lg_lte.cfg /root/Amarisoft/enb/config/enb.cfg
		;;
	sk_5g)
		ln -s /root/Amarisoft/enb/config/enb_sk_5g.cfg /root/Amarisoft/enb/config/enb.cfg
		;;
	kt_5g)
		ln -s /root/Amarisoft/enb/config/enb_kt_5g.cfg /root/Amarisoft/enb/config/enb.cfg
		;;
	lg_5g)
		ln -s /root/Amarisoft/enb/config/enb_lg_5g.cfg /root/Amarisoft/enb/config/enb.cfg
		;;
	*)
		touch /tmp/.err
		;;
esac
	
/etc/init.d/enb start
