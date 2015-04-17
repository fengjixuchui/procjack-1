[org 0]
[bits 64]

;pj!CInjectData::Package
;   +0x000 InitialCode      : [2048] UChar
;   +0x800 DllPath          : [260] Uint2B
;   +0xa08 peb              : Ptr64 Void
;   +0xa10 ntdll            : Ptr64 Void
;   +0xa18 kernel32         : Ptr64 Void
;   +0xa20 LoadLibraryW     : Ptr64 Void
;   +0xa28 FreeLibrary      : Ptr64 Void
;   +0xa30 GetProcAddress   : Ptr64 Void
;   +0xa38 CreateThread     : Ptr64 Void
;   +0xa40 ExitThread       : Ptr64 Void
;   +0xa48 WaitForSingleObject : Ptr64 Void
;   +0xa50 CloseHandle      : Ptr64 Void
;   +0xa58 Context          : [1] UChar

ShellCode:
push rbx
sub rsp,byte +0x30
mov rax,[gs:0x30]
mov rbx,rcx             ; rbx=Context
mov rdx,[rax+0x60]
mov [rcx+0xa08],rdx     ; rcx=Context, rdx=peb
call GetImageBase

mov rcx,[rbx+0xa10]     ; rcx=ntdll
mov rdx,rbx             ; rdx=Context
call GetProcAddress

mov rcx,[rbx+0xa18]     ; rcx=kernel32
mov rdx,rbx             ; rdx=Context
call GetProcAddress

mov     rcx,rbx
add     rcx,800h
call    [rbx+0A20h]     ; LoadLibrary
mov     rdi,rax
test    rax,rax
je      labelfd2f1789

mov     edx,064h        ; Ordinal
mov     rcx,rax
mov     [rsp+40h], rsi
call    [rbx+0A30h]     ; GetProcAddress
xor     ecx, ecx
mov     r9, rbx
mov     [rsp+28h], rcx
mov     r8, rax
xor     edx, edx
mov     [rsp+20h], ecx
call    [rbx+0A38h]     ; CreateThread
mov     rsi, rax
test    rax, rax
je      labelfd2f177b

or      edx, 0FFFFFFFFh
mov     rcx, rax
call    [rbx+0A48h]     ; WaitForSingleObject
mov     rcx, rsi
call    [rbx+0A50h]     ; CloseHandle

labelfd2f177b:
mov     rcx, rdi
call    [rbx+0A28h]     ; FreeLibrary
mov     rsi, [rsp+40h]

labelfd2f1789:
xor     ecx, ecx
call    [rbx+0A40h]     ; ExitThread

mov eax,0x2a
add rsp,byte +0x30
pop rbx
ret

int3
int3

GetImageBase:
mov rax,[rcx+0xa08]     ; rax = PEB
mov r8,rcx
mov r9,[rax+0x18]       ; PEB::Ldr
mov rdx,[r9+0x20]       ; PEB::Ldr->InMemoryOrderModuleList
add r9,byte +0x20
cmp rdx,r9
jz label0xeb

mov r10,[rcx+0xa10]

label0x26:
mov rax,[rdx+0x50]
test r10,r10
jnz label0x49

cmp dword [rax],0x74006e
jnz label0x49

cmp dword [rax+0x4],0x6c0064
jnz label0x49

cmp dword [rax+0x8],0x2e006c
jz label0x65

label0x49:
mov ecx,[rax]
cmp ecx,0x54004e
jnz label0x72

cmp dword [rax+0x4],0x4c0044
jnz label0x72

cmp dword [rax+0x8],0x2e004c
jnz label0x72

label0x65:
mov r10,[rdx+0x20]
mov [r8+0xa10],r10
jmp label0xdf

label0x72:
cmp qword [r8+0xa18],byte +0x0
jnz label0xa8

cmp ecx,0x45004b
jnz label0xa8

cmp dword [rax+0x4],0x4e0052
jnz label0xa8

cmp dword [rax+0x8],0x4c0045
jnz label0xa8

cmp dword [rax+0xc],0x320033
jnz label0xa8

cmp dword [rax+0x10],0x44002e
jz label0xd4

label0xa8:
cmp ecx,0x65006b
jnz label0xdf

cmp dword [rax+0x4],0x6e0072
jnz label0xdf

cmp dword [rax+0x8],0x6c0065
jnz label0xdf

cmp dword [rax+0xc],0x320033
jnz label0xdf

cmp dword [rax+0x10],0x64002e
jnz label0xdf

label0xd4:
mov rax,[rdx+0x20]
mov [r8+0xa18],rax

label0xdf:
mov rdx,[rdx]
cmp rdx,r9
jnz label0x26

label0xeb:
rep ret


int3
int3


GetProcAddress:
sub rsp,byte +0x8
mov eax,0x5a4d
mov r10,rcx
cmp [rcx],ax
jnz label0x355

mov r9d,[rcx+0x3c]
cmp dword [r9+rcx],0x4550
jnz label0x355

movzx r8d,word [r9+rcx+0x4]
mov r11d,0x14c
cmp r8w,r11w
jz label0x148

mov eax,0x8664
cmp r8w,ax
jnz label0x355

label0x148:
mov [rsp+0x10],rbx
cmp r8w,r11w
mov [rsp+0x20],rsi
mov [rsp],rdi
mov eax,0x70
mov ecx,0x60
cmovz eax,ecx
xor r11d,r11d
add rax,r9
mov r9d,[rax+r10+0x18]
add r9,r10
mov ebx,[r9+0x20]
mov edi,[r9+0x24]
mov esi,[r9+0x1c]
add rbx,r10
add rdi,r10
add rsi,r10
cmp [r9+0x18],r11d
jna label0x347

mov [rsp+0x18],rbp
mov rbp,[rdx+0xa20]

label0x1a0:
movzx ecx,word [rdi+r11*2]
mov eax,[rbx+r11*4]
mov r8d,[rsi+rcx*4]
add rax,r10
add r8,r10
test rbp,rbp
jnz label0x1e7

cmp dword [rax],0x64616f4c
jnz label0x1e7

cmp dword [rax+0x4],0x7262694c
jnz label0x1e7

cmp dword [rax+0x8],0x57797261
jnz label0x1e7

cmp [rax+0xc],bpl
jnz label0x1e7

mov rbp,r8
mov [rdx+0xa20],r8
jmp label0x335

label0x1e7:
cmp qword [rdx+0xa28],byte +0x0
jnz label0x217

cmp dword [rax],0x65657246
jnz label0x217

cmp dword [rax+0x4],0x7262694c
jnz label0x217

cmp dword [rax+0x8],0x797261
jnz label0x217

mov [rdx+0xa28],r8
jmp label0x335

label0x217:
cmp qword [rdx+0xa30],byte +0x0
jnz label0x258

cmp dword [rax],0x50746547
jnz label0x258

cmp dword [rax+0x4],0x41636f72
jnz label0x258

cmp dword [rax+0x8],0x65726464
jnz label0x258

mov ecx,[rax+0xc]
and ecx,0xffffff
cmp ecx,0x7373
jnz label0x258

mov [rdx+0xa30],r8
jmp label0x335

label0x258:
cmp qword [rdx+0xa38],byte +0x0
jnz label0x28e

cmp dword [rax],0x61657243
jnz label0x28e

cmp dword [rax+0x4],0x68546574
jnz label0x28e

cmp dword [rax+0x8],0x64616572
jnz label0x28e

cmp byte [rax+0xc],0x0
jnz label0x28e

mov [rdx+0xa38],r8
jmp label0x335

label0x28e:
cmp qword [rdx+0xa40],byte +0x0
jnz label0x2cb

cmp dword [rax],0x456c7452
jnz label0x2cb

cmp dword [rax+0x4],0x55746978
jnz label0x2cb

cmp dword [rax+0x8],0x54726573
jnz label0x2cb

cmp dword [rax+0xc],0x61657268
jnz label0x2cb

cmp word [rax+0x10],byte +0x64
jnz label0x2cb

mov [rdx+0xa40],r8
jmp label0x335

label0x2cb:
cmp qword [rdx+0xa48],byte +0x0
jnz label0x30a

cmp dword [rax],0x74696157
jnz label0x30a

cmp dword [rax+0x4],0x53726f46
jnz label0x30a

cmp dword [rax+0x8],0x6c676e69
jnz label0x30a

cmp dword [rax+0xc],0x6a624f65
jnz label0x30a

cmp dword [rax+0x10],0x746365
jnz label0x30a

mov [rdx+0xa48],r8
jmp label0x335

label0x30a:
cmp qword [rdx+0xa50],byte +0x0
jnz label0x335

cmp dword [rax],0x736f6c43
jnz label0x335

cmp dword [rax+0x4],0x6e614865
jnz label0x335

cmp dword [rax+0x8],0x656c64
jnz label0x335

mov [rdx+0xa50],r8

label0x335:
inc r11d
cmp r11d,[r9+0x18]
jc label0x1a0

mov rbp,[rsp+0x18]

label0x347:
mov rsi,[rsp+0x20]
mov rbx,[rsp+0x10]
mov rdi,[rsp]

label0x355:
add rsp,byte +0x8
ret


int3
int3