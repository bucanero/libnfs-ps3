#ifndef SYS_STATVFS_H
#define SYS_STATVFS_H

#include <sys/types.h> /* for fsblkcnt_t, fsfilcnt_t */

#include <stdint.h>
/* Default to 32 bit file sizes on newlib platforms */
typedef uint32_t fsblkcnt_t;
typedef uint32_t fsfilcnt_t;

#ifdef __cplusplus
extern "C" {
#endif
 
struct statvfs {
     unsigned long f_bsize;   
     unsigned long f_frsize;  
     fsblkcnt_t f_blocks;     
     fsblkcnt_t f_bfree;      
     fsblkcnt_t f_bavail;     
     fsfilcnt_t f_files;      
     fsfilcnt_t f_ffree;      
     fsfilcnt_t f_favail;     
     unsigned long f_fsid;    
     unsigned long f_flag;    
     unsigned long f_namemax; 
};
 
enum {
     ST_RDONLY = 1,        /* Mount read-only.  */
     ST_NOSUID = 2,        /* Ignore suid and sgid bits.  */
};
 
#ifdef __cplusplus
}
#endif
 
#endif /* SYS_STATVFS_H */
