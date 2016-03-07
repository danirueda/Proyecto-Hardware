.arm
.global DoUndef
.global DoDabort

DoUndef:	.word 0xE6000010
			mov pc,lr


DoDabort:	ldr r0,=0x0a333333
			str r0,[r0]
			mov pc,lr

.end
