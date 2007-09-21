#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__

__inline void DWSet ( PVOID dst, DWORD data, DWORD len )
{
	__asm
	{
		MOV EDI, dst
		MOV EAX, data
		MOV ECX, len

		REP STOSD
	}
}

__inline void DWCopy ( PVOID dst, CONST VOID *src, DWORD len )
{
	__asm
	{
		MOV EDI, dst
		MOV ESI, src
		MOV ECX, len

		REP MOVSD
	}
}

__inline void DWCopyCK ( PVOID dst, CONST VOID *src, DWORD len, DWORD cK )
{
	__asm
	{
		MOV EDI, dst
		MOV ESI, src
		MOV ECX, len
		MOV EDX, cK
		
		JMP l3
l1:
		ADD EDI, 4
		ADD ESI, 4
l2:
		DEC ECX
l3:
		CMP ECX, 0
		JE l4

		MOV EAX, [ESI]

		CMP EAX, EDX
		JE l1

		MOVSD

		JMP l2
l4:
	}
}

#endif // __ASSEMBLER_H__