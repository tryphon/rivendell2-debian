#
# Regular cron jobs for the rivendell package
#
0 4	* * *	root	[ -x /usr/bin/rivendell_maintenance ] && /usr/bin/rivendell_maintenance
