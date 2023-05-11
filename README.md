# effective-waffle
yet another sleep encryption thing. also used the default github repo name for this one.

TpAllocWork + TpPostWork + TpWaitForWork + TpReleaseWork to create + execute + clean the callback.  

The callback is some scuffed assembly that queues up a ton of rop gadgets that will pop arguments (either hard coded values, or based on an offset within a struct passed as the TpAllocWork parameter) into the correct registers, then returning to the next gadget/function. Padding is creating between some function calls via pushing 5 garbage values + a gadget to move the stack pointer by 40 bytes as a hacky solution to fix some weird stack stuff I ran into. This is also probably why the call stack looks disgusting but idk how else to address that. 

## Credits
Kudaes, for creating [RustChain](https://github.com/Kudaes/RustChain) which the ROP idea came from for me  
C5pider, for creating [Ekko](https://github.com/Cracked5pider/Ekko/) and helping me understand sleep encryption  
x86natthew, for helping me troubleshoot weird stack stuff  
ParanoidNinja, for providing a [blog post](https://0xdarkvortex.dev/hiding-in-plainsight/) on the Tp Apis  
