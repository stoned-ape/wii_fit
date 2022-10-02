syscall::open:entry{ 
	printf("%s %s",execname,copyinstr(arg0)); 
}

syscall::open_nocancel:entry{ 
	printf("%s %s",execname,copyinstr(arg0)); 
}

syscall::openat:entry{ 
	printf("%s %s",execname,copyinstr(arg1)); 
}

syscall::openat_nocancel:entry{ 
	printf("%s %s",execname,copyinstr(arg1)); 
}

syscall::open*:return{ 
	printf("%s %d",execname,arg0); 
}
