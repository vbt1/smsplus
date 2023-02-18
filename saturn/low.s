   .section SEGA_P
   .align   4

!   .global  _memcpy
!   .global  _memcpyb
   .global  _memcpyw
   .global  _memcpyl

! dst 	   = r4
! src 	   = r5
! size     = r6
!_memcpy:
!_memcpyb:
!   tst    r6,r6
!   bt     lendb

!lmemcpy:
!   mov.b  @r5+,r0
!   mov.b  r0,@r4
!   dt     r6
!   add    #1,r4
!   bf     lmemcpy
!lendb:
!   rts
!   nop

_memcpyw:
   shlr   r6
   tst    r6,r6
   bt     lendw

lmemcpyw:
   mov.w  @r5+,r0
   mov.w  r0,@r4
   dt     r6
   add    #2,r4
   bf     lmemcpyw

lendw:
   rts
   nop

_memcpyl:
   shlr2   r6
   tst    r6,r6
   bt     lendl

lmemcpyl:
   mov.l  @r5+,r0
   mov.l  r0,@r4
   dt     r6
   add    #4,r4
   bf     lmemcpyl

lendl:
   rts
   nop

   .end
