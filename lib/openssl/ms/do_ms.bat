
perl util\mkfiles.pl >MINFO
rem perl util\mk1mf.pl VC-MSDOS no-sock >ms\msdos.mak
rem perl util\mk1mf.pl VC-W31-32 >ms\w31.mak
perl util\mk1mf.pl debug dll VC-W31-32 >ms\w31dll.mak
perl util\mk1mf.pl debug no-asm VC-WIN32 >ms\nt.mak
perl util\mk1mf.pl debug dll no-asm VC-WIN32 >ms\ntdll.mak

perl util\mkdef.pl debug 16 libeay > ms\libeay16.def
perl util\mkdef.pl debug 32 libeay > ms\libeay32.def
perl util\mkdef.pl debug 16 ssleay > ms\ssleay16.def
perl util\mkdef.pl debug 32 ssleay > ms\ssleay32.def
