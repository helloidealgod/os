#ifndef _ELF_H
#define _ELF_H

/* elf32_head: define the ELF header */
#define EI_NIDENT   (16)

struct elf32_head {
    unsigned char e_ident[EI_NIDENT];   /* the magic number  */
    unsigned short e_type;              /* identify the object file type */
    unsigned short e_machine;           /* specify the machine architechture */
    unsigned int e_version;             /* identify the object file version*/
    unsigned int e_entry;               /* entry point address of the process */
    unsigned int e_phoff;               /* start of program header */
    unsigned int e_shoff;               /* start of section header */
    unsigned int e_flags;               /* flags */
    unsigned short e_ehsize;            /* size of the ELF header */
    unsigned short e_phentsize;         /* size of program header */
    unsigned short e_phnum;             /* number of program header */
    unsigned short e_shentsize;         /* size of section header */
    unsigned short e_shnum;             /* number of section header */
    unsigned short e_shstrndx;          /* section header string table index */
};

#endif