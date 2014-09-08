#DEPENDENCIES
* Linux kernel should be configured with "Packet Socket" support.
* TCL/TIX utilities requires udptcl library (http://sourceforge.net/projects/tcludp/).
* Application for RAW video visualization requires xvideo extention to work and X11 devel headers to build. 

#PROJECT TREE
	tools/platforms/cam4-client:
		.i686/cam4-jpeg-data-cl
			Example of client to capture FACES and MJPEG data via network
			DEV ABI interface.

		.i686/cam4_ps
			Example of client to capture RAW FACES and RAW VIDEO data via network
			RAW VIDEO ABI interface.

			This application is subject to deep refactoring and its parts quality are
			very variable.

		.i686/cam4_ps_Xclient
			Example of client to visualize data captured by .i686/cam4_ps

			This application is subject to deep refactoring and its parts quality are
			very variable.

	tools/platforms/cam4-tools:
		.i686/cam4-tcpdump
			Utility to simple analize and dump RAW VIDEO from wireshark/tcpdump capture
			files in .tcpdump format
