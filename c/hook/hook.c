#include <stdio.h>
#include <stdlib.h>
#include <asm/ptrace.h>
#include <asm/user.h>
#include <asm/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <android/log.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>

#define MAX_PATH 0x100
#define REMOTE_ADDR( addr, local_base, remote_base ) ( (uint32_t)(addr) + (uint32_t)(remote_base) - (uint32_t)(local_base) )

/* write the assembler code into target proc,
 * and invoke it to execute
 */
int writecode_to_targetproc( 
        pid_t target_pid, // target process pid
        const char *library_path, // the path of .so that will be 
        // upload to target process 
        const char *function_name, // .so init fucntion e.g. hook_init
        void *param, // the parameters of init function
        size_t param_size ) // number of parameters 
{
    int ret = -1;
    void *mmap_addr, *dlopen_addr, *dlsym_addr, *dlclose_addr;
    void *local_handle, *remote_handle, *dlhandle;
    uint8_t *map_base;
    uint8_t *dlopen_param1_ptr, *dlsym_param2_ptr, *saved_r0_pc_ptr, *inject_param_ptr, *remote_code_ptr, *local_code_ptr;

    struct pt_regs regs, original_regs;

    // extern global variable in the assembler code 
    extern uint32_t _dlopen_addr_s, _dlopen_param1_s, _dlopen_param2_s, \
        _dlsym_addr_s, _dlsym_param2_s, _dlclose_addr_s, \
        _inject_start_s, _inject_end_s, _inject_function_param_s, \
        _saved_cpsr_s, _saved_r0_pc_s;

    uint32_t code_length;

    long parameters[10];

    // make target_pid as its child process and stop
    if ( ptrace_attach( target_pid ) == -1 )
        return -1;

    // get the values of 18 registers from target_pid
    if ( ptrace_getregs( target_pid, &regs ) == -1 )
        goto exit;

    // save original registers 
    memcpy( &original_regs, @regs, sizeof(regs) );

    // get mmap address from target_pid
    // the mmap is the address of mmap in the cur process
    mmap_addr = get_remote_addr( target_pid, "/system/lib/libc.so", (void *)mmap );

    // set mmap parameters
    parameters[0] = 0;  // addr
    parameters[1] = 0x4000; // size
    parameters[2] = PROT_READ | PROT_WRITE | PROT_EXEC;  // prot
    parameters[3] =  MAP_ANONYMOUS | MAP_PRIVATE; // flags
    parameters[4] = 0; //fd
    parameters[5] = 0; //offset

    // execute the mmap in target_pid
    if ( ptrace_call( target_pid, (uint32_t)mmap_addr, parameters, 6, &regs) == -1 )
        goto exit;

    // get the return values of mmap <in r0>
    if ( ptrace_getregs( target_pid, &regs) == -1 )
        goto exit;

    // get the start address for assembler code
    map_base = (uint8_t *)regs.ARM_r0;

    // get the address of dlopen, dlsym and dlclose in target process
    dlopen_addr = get_remote_addr( target_pid, "/system/bin/linker", (void *)dlopen );
    dlsym_addr = get_remote_addr( target_pid, "/system/bin/linker", (void *)dlsym );
    dlclose_addr = get_remote_addr( target_pid, "/system/bin/linker", (void *)dlclose );

    // set the start address for assembler code in target process
    remote_code_ptr = map_base + 0x3C00;

    // set the start address for assembler code in cur process
    local_code_ptr = (uint8_t *)&_inject_start_s;

    // set global variable of assembler code
    // and these address is in the target process
    _dlopen_addr_s = (uint32_t)dlopen_addr;
    _dlsym_addr_s = (uint32_t)dlsym_addr;
    _dlclose_addr_s = (uint32_t)dlclose_addr;

    code_length = (uint32_t)&_inject_end_s - (uint32_t)&_inject_start_s;

    dlopen_param1_ptr = local_code_ptr + code_length + 0x20;
    dlsym_param2_ptr = dlopen_param1_ptr + MAX_PATH;
    saved_r0_pc_ptr = dlsym_param2_ptr + MAX_PATH;
    inject_param_ptr = saved_r0_pc_ptr + MAX_PATH;


    // save library path to assembler code global variable
    strcpy( dlopen_param1_ptr, library_path );
    _dlopen_param1_s = REMOTE_ADDR( dlopen_param1_ptr, local_code_ptr, remote_code_ptr );


    // save function name to assembler code global variable
    strcpy( dlsym_param2_ptr, function_name );
    _dlsym_param2_s = REMOTE_ADDR( dlsym_param2_ptr, local_code_ptr, remote_code_ptr );

    // save cpsr to assembler code global variable
    _saved_cpsr_s = original_regs.ARM_cpsr;

    // save r0-r15 to assembler code global variable
    memcpy( saved_r0_pc_ptr, &(original_regs.ARM_r0), 16 * 4 ); // r0 ~ r15
    _saved_r0_pc_s = REMOTE_ADDR( saved_r0_pc_ptr, local_code_ptr, remote_code_ptr );

    // save function parameters to assembler code global variable
    memcpy( inject_param_ptr, param, param_size );
    _inject_function_param_s = REMOTE_ADDR( inject_param_ptr, local_code_ptr, remote_code_ptr );

    // write the assembler code into target process
    // now the values of global variable is in the target process space
    ptrace_writedata( target_pid, remote_code_ptr, local_code_ptr, 0x400 );

    memcpy( &regs, &original_regs, sizeof(regs) );

    // set sp and pc to the start address of assembler code
    regs.ARM_sp = (long)remote_code_ptr;
    regs.ARM_pc = (long)remote_code_ptr;

    // set registers for target process
    ptrace_setregs( target_pid, &regs );

    // make the target_pid is not a child process of cur process
    // and make target_pid continue to running
    ptrace_detach( target_pid );

    // now finish it successfully
    ret = 0;

exit:
    return ret;
}

int ptrace_attach( pid_t pid )
{
    // after PTRACE_ATTACH, the proc<pid> will stop
    if ( ptrace( PTRACE_ATTACH, pid, NULL, 0  ) < 0 )
    {
        perror( "ptrace_attach" );
        return -1;
    }

    // wait proc<pid> stop
    waitpid( pid, NULL, WUNTRACED );

    // after PTRACE_SYSCALL, the proc<pid> will continue,
    // but when exectue sys call function, proc<pid> will stop
    if ( ptrace( PTRACE_SYSCALL, pid, NULL, 0  ) < 0 )
    {
        perror( "ptrace_syscall" );
        return -1;
    }

    // wait proc<pid> stop
    waitpid( pid, NULL, WUNTRACED );

    return 0;
}


int ptrace_getregs( pid_t pid, struct pt_regs* regs )
{
    if ( ptrace( PTRACE_GETREGS, pid, NULL, regs ) < 0 )
    {
        perror( "ptrace_getregs: Can not get register values" );
        return -1;
    }

    return 0;
}


/* find the start address of module whose name is module_name 
 * in the designated process
 */
void* get_module_base( pid_t pid, const char* module_name )
{
    FILE *fp;
    long addr = 0;
    char *pch;
    char filename[32];
    char line[1024];

    if ( pid < 0 )
    {
        /* self process */
        snprintf( filename, sizeof(filename), "/proc/self/maps", pid );
    }
    else
    {
        snprintf( filename, sizeof(filename), "/proc/%d/maps", pid );
    }

    fp = fopen( filename, "r" );

    if ( fp != NULL )
    {
        while ( fgets( line, sizeof(line), fp ) )
        {
            if ( strstr( line, module_name ) )
            {
                pch = strtok( line, "-" );
                addr = strtoul( pch, NULL, 16 );

                if ( addr == 0x8000 )
                    addr = 0;

                break;
            }
        }
        fclose( fp ) ;
    }

    return (void *)addr;
}

void* get_remote_addr( pid_t target_pid, const char* module_name, void* local_addr )
{
    void* local_handle, *remote_handle;

    local_handle = get_module_base( -1, module_name );
    remote_handle = get_module_base( target_pid, module_name );

    return (void *)( (uint32_t)local_addr - (uint32_t)local_handle + (uint32_t)remote_handle );
}

int ptrace_call( pid_t pid, uint32_t addr, long *params, uint32_t num_params, struct pt_regs* regs )
{
    uint32_t i;

    // put the first 4 parameters into r0-r3
    for ( i = 0; i < num_params && i < 4; i ++ )
    {
        regs->uregs[i] = params[i];
    }

    // push remained params into stack
    if ( i < num_params )
    {
        regs->ARM_sp -= (num_params - i) * sizeof(long) ;
        ptrace_writedata( pid, (void *)regs->ARM_sp, (uint8_t *)params[i], (num_params - i) * sizeof(long) );
    }
    // set the pc to func <e.g: mmap> that will be executed
    regs->ARM_pc = addr;
    if ( regs->ARM_pc & 1 )
    {
        /* thumb */
        regs->ARM_pc &= (~1u);
        regs->ARM_cpsr |= CPSR_T_MASK;
    }
    else
    {
        /* arm */
        regs->ARM_cpsr &= ~CPSR_T_MASK;
    }

    // when finish this func, pid will stop
    regs->ARM_lr = 0;   

    // set the regsister and start to execute
    if ( ptrace_setregs( pid, regs ) == -1 
            || ptrace_continue( pid ) == -1 )
    {
        return -1;
    }

    // wait pid finish work and stop
    waitpid( pid, NULL, WUNTRACED );

    return 0;
}


int ptrace_writedata( pid_t pid, uint8_t *dest, uint8_t *data, size_t size )
{
    uint32_t i, j, remain;
    uint8_t *laddr;

    union u {
        long val;
        char chars[sizeof(long)];
    } d;

    j = size / 4;
    remain = size % 4;

    laddr = data;

    for ( i = 0; i < j; i ++ )
    {
        memcpy( d.chars, laddr, 4 );
        ptrace( PTRACE_POKETEXT, pid, dest, d.val );

        dest  += 4;
        laddr += 4;
    }

    if ( remain > 0 )
    {
        d.val = ptrace( PTRACE_PEEKTEXT, pid, dest, 0 );
        for ( i = 0; i < remain; i ++ )
        {
            d.chars[i] = *laddr ++;
        }

        ptrace( PTRACE_POKETEXT, pid, dest, d.val );

    }

    return 0;
}


int ptrace_setregs( pid_t pid, struct pt_regs* regs )
{
    if ( ptrace( PTRACE_SETREGS, pid, NULL, regs ) < 0 )
    {
        perror( "ptrace_setregs: Can not set register values" );
        return -1;
    }

    return 0;
}


int ptrace_detach( pid_t pid )
{
    if ( ptrace( PTRACE_DETACH, pid, NULL, 0 ) < 0 )
    {
        perror( "ptrace_detach" );
        return -1;
    }

    return 0;
}

int g_isInit = 0;    
pthread_t g_hThread;     

__attribute__((visibility("default"))) void hook_init( char *args )
{
    if( g_isInit == 1 )
    {
        printf("i am already in!");
        return;
    }

    void* soHandle = NULL;

    // the libapp.so is a .so of target process, and it call strcmp
    soHandle  = dlopen( "libapp.so", RTLD_GLOBAL );
    if( soHandle != NULL )
    {
        g_realstrcmp = NULL;
        replaceFunc( soHandle, "strcmp", my_strcmp, (void**)&g_realstrcmp );

        int ret = pthread_create( &g_hThread, NULL, my_thread, NULL );
        if( ret != 0 )
        {
            printf("create thread error:%d", ret );
        }

        g_isInit = 1;
    }

}

// replace function of libapp.so
// e.g: replace strcmp of libapp.so with my_strcmp
void replaceFunc(void *handle,const char *name, void* pNewFun, void** pOldFun )
{

    if(!handle)
        return;

    soinfo *si = (soinfo*)handle;   
    Elf32_Sym *symtab = si->symtab;  
    const char *strtab = si->strtab;  
    Elf32_Rel *rel = si->plt_rel;
    unsigned count = si->plt_rel_count; 
    unsigned idx; 

    // these external functions that are called by libapp.so 
    // is in the plt_rel
    for(idx=0; idx<count; idx++) 
    {  
        unsigned type = ELF32_R_TYPE(rel->r_info);  
        unsigned sym = ELF32_R_SYM(rel->r_info);  
        unsigned reloc = (unsigned)(rel->r_offset + si->base);  
        char *sym_name = (char *)(strtab + symtab[sym].st_name); 

        if(strcmp(sym_name, name)==0) 
        { 
            *pOldFun = (void *)*((unsigned*)reloc); 
            *((unsigned*)reloc)= pNewFun;
            break;
        } 
        rel++;  
    } 
}


// global function variable, save the address of strcmp of libapp.so
int (*g_realstrcmp)(const char *s1, const char *s2);

// my strcmp function
int my_strcmp(const char *s1, const char *s2)
{
    if( g_realstrcmp != NULL )
    {
        int nRet = g_realstrcmp( s1, s2 );
        printf("***%s: s1=%s, s2=%s\n",__FUNCTION__, s1, s2 );
        return nRet;
    }

    return -1;
}


// create a thread
void* my_thread( void* pVoid )
{
    int sock;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if( sock < -1 )
    {
        printf("create socket failed!\n");
        return 0;
    }

    struct sockaddr_in addr_serv;  
    int len;  
    memset(&addr_serv, 0, sizeof(struct sockaddr_in));  
    addr_serv.sin_family = AF_INET;  
    addr_serv.sin_port = htons(9999);   
    addr_serv.sin_addr.s_addr = inet_addr("127.0.0.1");  
    len = sizeof(addr_serv);  

    int flags = fcntl( sock, F_GETFL, 0); 
    fcntl( sock, F_SETFL, flags | O_NONBLOCK);
    int nPreState = -1;
    unsigned char data=0;
    while( 1 )
    {
        data++;
        sendto( sock, &data,  sizeof( data ), 0, (struct sockaddr *)&addr_serv, sizeof( addr_serv ) );
        usleep( 30000 );
    }
}

