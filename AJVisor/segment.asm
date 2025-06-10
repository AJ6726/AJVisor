.code

__readcs proc
	mov ax, cs
	ret
__readcs endp

__readss proc
	mov ax, ss
	ret
__readss endp

__readds proc
	mov ax, ds
	ret
__readds endp

__reades proc
	mov ax, es
	ret
__reades endp

__readfs proc
	mov ax, fs
	ret
__readfs endp

__readgs proc
	mov ax, gs
	ret
__readgs endp

__readtr proc
	str ax
	ret
__readtr endp

__readldtr proc
	sldt ax
	ret
__readldtr endp

__writeds proc
    mov ds, cx
	ret
__writeds endp

__writees proc
	mov es, cx
	ret
__writees endp

__writefs proc
	mov fs, cx
	ret
__writefs endp

__writegs proc
	mov gs, cx
	ret
__writegs endp

__writetr proc
	ltr cx
	ret
__writetr endp

__writeldtr proc
	lldt cx
	ret
__writeldtr endp

end