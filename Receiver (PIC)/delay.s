;Delay Example
;Jason Losh

;------------------------------------------------------------------------------
; Objectives and notes             
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Device includes and assembler directives             
;------------------------------------------------------------------------------

          .include "p33FJ128MC802.inc"
          .global _wait_us
          .global _wait_ms
          .global _wait_52083ns

;------------------------------------------------------------------------------
; Code starts here               
;------------------------------------------------------------------------------

           .text

;------------------------------------------------------------------------------
; Subroutines                
;------------------------------------------------------------------------------

;Wait_us (exactly including W0 load, call, and return)
;  Parameters: time in us in W0
;  Returns:    nothing
_wait_us:                                     ;need 1us x 40 MHz instr cycle 
                                              ;= 40 ticks / us
                                              ;make N = W0 when fn called
                                              ;calculate 40 ticks if N = 1
                                              ; 1 tick for mov #__, W0
                                              ; 2 ticks to get here (call)
           repeat   #29                       ; 1 tick
           nop                                ; 40 - 10 = 30 ticks (calculated)

wu_loop:   dec      W0, W0                    ; N ticks  
           bra      Z, wu_end                ; 2 + (N-1) ticks 
 
           repeat   #34                       ; (N-1) ticks
           nop                                ; 35(N-1) ticks
           bra      wu_loop                   ; 2(N-1) ticks 

wu_end:    return                             ; 3 ticks
                                       
;Wait_ms (approx)
;  Parameters: time in ms in W0
;  Returns:    nothing
_wait_ms:  mov      W0, W1                    ;store time in 1ms resolution
wm_loop:   mov      #1000, W0                 ;wait approx 1ms
           call     _wait_us       
           dec      W1, W1      
           bra      NZ, wm_loop    
           return                   


_wait_52083ns:
    
           repeat #1660
           nop
           return                          
          .end

