#ifndef _GLOBAL_MACROS_H_
#define _GLOBAL_MACROS_H_

#define KSEG0(addr)     (addr)
#define KSEG1(addr)     (addr)

#define REG_ACCESS_ADDRESS(addr)    KSEG1(addr)

#define REG8  volatile unsigned char
#define REG16 volatile unsigned short
#define REG32 volatile unsigned int
#define REG64 volatile unsigned long long

#define UINT8  unsigned char 
#define UINT16 unsigned short 
#define UINT32 unsigned int
#define UINT64 unsigned long long

#define REG_READ_UINT8( _reg_ )           (*(volatile unsigned char*)(_reg_))
#define REG_WRITE_UINT8( _reg_, _val_)    ((*(volatile unsigned char*)(_reg_)) = (unsigned char)(_val_))

#define REG_READ_UINT32( _reg_ )           (*(volatile unsigned int*)(_reg_))
#define REG_WRITE_UINT32( _reg_, _val_)    ((*(volatile unsigned int*)(_reg_)) = (unsigned int)(_val_))

#define REG_READ_U64( _reg_ )          (*(volatile unsigned long*)(_reg_))
#define REG_WRITE_U64( _reg_, _val_)   ((*(volatile unsigned long*)(_reg_)) = (unsigned long)(_val_))

#endif
