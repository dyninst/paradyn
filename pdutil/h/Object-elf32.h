/************************************************************************
 * Object-elf32.h: ELF-32 object files.
************************************************************************/





#if !defined(_Object_elf32_h_)
#define _Object_elf32_h_





/************************************************************************
 * header files.
************************************************************************/

#include <util/h/Dictionary.h>
#include <util/h/Line.h>
#include <util/h/String.h>
#include <util/h/Symbol.h>
#include <util/h/Types.h>
#include <util/h/Vector.h>

extern "C" {
#include <libelf.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
}





/************************************************************************
 * class Object
************************************************************************/

class Object : public AObject {
public:
             Object (const char *, void (*)(const char *) = log_msg);
             Object (const Object &);
    virtual ~Object ();

    Object&    operator= (const Object &);

private:
    static
    void    log_elferror (void (*)(const char *), const char *);

    bool      loaded_elf (int, bool &, Elf* &, Elf32_Ehdr* &, Elf32_Phdr* &,
                          unsigned &, Elf_Scn* &, Elf_Scn* &);
    void     load_object ();
};

inline
Object::Object(const char* file, void (*err_func)(const char *))
    : AObject(file, err_func) {
    load_object();
}

inline
Object::Object(const Object& obj)
    : AObject(obj) {
    load_object();
}

inline
Object::~Object() {
}

inline
Object&
Object::operator=(const Object& obj) {
    (void) AObject::operator=(obj);
    return *this;
}

inline
void
Object::log_elferror(void (*pfunc)(const char *), const char* msg) {
    const char* err = elf_errmsg(elf_errno());
    log_printf(pfunc, "%s: %s\n", msg, err ? err : "(bad elf error)");
}

inline
bool
Object::loaded_elf(int fd, bool& did_elf, Elf*& elfp, Elf32_Ehdr*& ehdrp,
    Elf32_Phdr*& phdrp, unsigned& txtaddr,
    Elf_Scn*& symscnp, Elf_Scn*& strscnp) {

    elf_version(EV_CURRENT);
    elf_errno();

    if (((elfp = elf_begin(fd, ELF_C_READ, 0)) == 0)
        || (elf_kind(elfp) != ELF_K_ELF)) {
        log_elferror(err_func_, "opening file");
        return false;
    }
    did_elf = true;

    if (((ehdrp = elf32_getehdr(elfp)) == 0)
        || (ehdrp->e_ident[EI_CLASS] != ELFCLASS32)
        || (ehdrp->e_type != ET_EXEC)
        || (ehdrp->e_phoff == 0)
        || (ehdrp->e_shoff == 0)
        || (ehdrp->e_phnum == 0)
        || (ehdrp->e_shnum == 0)) {
        log_elferror(err_func_, "loading eheader");
        return false;
    }

    if ((phdrp = elf32_getphdr(elfp)) == 0) {
        log_elferror(err_func_, "loading pheader");
        return false;
    }

    Elf_Scn*    shstrscnp  = 0;
    Elf32_Shdr* shstrshdrp = 0;
    Elf_Data*   shstrdatap = 0;
    if (((shstrscnp = elf_getscn(elfp, ehdrp->e_shstrndx)) == 0)
        || ((shstrshdrp = elf32_getshdr(shstrscnp)) == 0)
        || ((shstrdatap = elf_getdata(shstrscnp, 0)) == 0)) {
        log_elferror(err_func_, "loading header section");
        return false;
    }

    const char* shnames = (const char *) shstrdatap->d_buf;
    Elf_Scn*    scnp    = 0;
    while ((scnp = elf_nextscn(elfp, scnp)) != 0) {
        Elf32_Shdr* shdrp = elf32_getshdr(scnp);
        if (!shdrp) {
            log_elferror(err_func_, "scanning sections");
            return false;
        }

        const char* TEXT_NAME   = ".text";
        const char* SYMTAB_NAME = ".symtab";
        const char* STRTAB_NAME = ".strtab";
        const char* name        = (const char *) &shnames[shdrp->sh_name];
        if (strcmp(name, TEXT_NAME) == 0) {
            txtaddr = shdrp->sh_addr;
        }
        else if (strcmp(name, SYMTAB_NAME) == 0) {
            symscnp = scnp;
        }
        else if (strcmp(name, STRTAB_NAME) == 0) {
            strscnp = scnp;
        }
    }
    if (!txtaddr || !symscnp || !strscnp) {
        log_elferror(err_func_, "no text/symbol/string section");
        return false;
    }

    return true;
}

inline
void
Object::load_object() {
    const char* file = file_.string_of();
    struct stat st;
    int         fd   = -1;
    char*       ptr  = 0;
    Elf*        elfp = 0;

    bool        did_open = false;
    bool        did_elf  = false;

    /* try */ {
        if (((fd = open(file, O_RDONLY)) == -1) || (fstat(fd, &st) == -1)) {
            log_perror(err_func_, file);
            /* throw exception */ goto cleanup;
        }
        did_open = true;

        if ((ptr = (char *) mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0))
            == (char *) -1) {
            log_perror(err_func_, "mmap");
            /* throw exception */ goto cleanup;
        }

        Elf32_Ehdr* ehdrp   = 0;
        Elf32_Phdr* phdrp   = 0;
        Elf_Scn*    symscnp = 0;
        Elf_Scn*    strscnp = 0;
        unsigned    txtaddr = 0;
        if (!loaded_elf(fd, did_elf, elfp, ehdrp, phdrp, txtaddr,
            symscnp, strscnp)) {
            /* throw exception */ goto cleanup;
        }

        for (unsigned i = 0; i < ehdrp->e_phnum; i++) {
            if ((phdrp[i].p_vaddr <= txtaddr)
                && ((phdrp[i].p_vaddr+phdrp[i].p_memsz) >= txtaddr)) {
                code_ptr_ = (Word *) &ptr[phdrp[i].p_offset];
                code_off_ = (Address) phdrp[i].p_vaddr;
                code_len_ = (unsigned) phdrp[i].p_memsz / sizeof(Word);
            }
        }
        if (!code_ptr_ || !code_off_ || !code_len_) {
            log_printf(err_func_, "cannot locate instructions\n");
            /* throw exception */ goto cleanup;
        }

        Elf_Data* symdatap = elf_getdata(symscnp, 0);
        Elf_Data* strdatap = elf_getdata(strscnp, 0);
        if (!symdatap || !strdatap) {
            log_elferror(err_func_, "locating symbol/string data");
            /* throw exception */ goto cleanup;
        }

        Elf32_Sym*  syms   = (Elf32_Sym *) symdatap->d_buf;
        unsigned    nsyms  = symdatap->d_size / sizeof(Elf32_Sym);
        const char* strs   = (const char *) strdatap->d_buf;
        string      module = "DEFAULT_MODULE";
        string      name   = "DEFAULT_NAME";
        for (i = 0; i < nsyms; i++) {
            Symbol::SymbolLinkage linkage =
                ((ELF32_ST_BIND(syms[i].st_info) == STB_LOCAL)
                ? Symbol::SL_LOCAL
                : Symbol::SL_GLOBAL);

            Symbol::SymbolType type = Symbol::ST_UNKNOWN;
            switch (ELF32_ST_TYPE(syms[i].st_info)) {
            case STT_FILE:
                module = string(&strs[syms[i].st_name]);
                type   = Symbol::ST_MODULE;
                break;

            case STT_OBJECT:
                type = Symbol::ST_OBJECT;
                break;

            case STT_FUNC:
                type = Symbol::ST_FUNCTION;
                break;

            default:
                continue;
            }

            name = string(&strs[syms[i].st_name]);
            symbols_[name] = Symbol(name, module, type, linkage,
                                    syms[i].st_value);
        }
    }

    /* catch */
cleanup: {
        if (did_elf && (elf_end(elfp) != 0)) {
            log_elferror(err_func_, "closing file");
        }
        if (did_open && (close(fd) == -1)) {
            log_perror(err_func_, "close");
        }
    }
}





#endif /* !defined(_Object_elf32_h_) */
