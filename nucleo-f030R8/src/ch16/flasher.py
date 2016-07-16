from __future__ import print_function

import sys, serial, binascii, time, struct, thread
from intelhex import IntelHex
from Crypto.Cipher import AES

#The AES key used to encrypt exchanged firmware
AES_KEY = "4D:61:73:74:65:72:69:6E:67:20:20:53:54:4D:33:32"

#We convert the key from the hexadecimal form to binary data
AES_KEY = binascii.unhexlify(AES_KEY.replace(":", ""))
AESEncryptor = AES.new(AES_KEY, AES.MODE_ECB)

ACK       = 0x79
NACK      = 0x1F
CMD_ERASE = 0x43
CMD_GETID = 0x02
CMD_WRITE = 0x2b

STM32_TYPE = {
    0x433: "STM32F401RE",
    0x440: "STM32F030R8"
}

class ProgramModeError(Exception):
    pass

class TimeoutError(Exception):
    pass

class STM32Flasher(object):
    def __init__(self, serialPort, baudrate=38400):
        self.serial = serial.Serial(serialPort, baudrate=baudrate, timeout=20)

    def _crc_stm32(self, data):
        #Computes CRC checksum using CRC-32 polynomial 
        crc = 0xFFFFFFFF

        for d in data:
            crc ^= d
            for i in range(32):
                if crc & 0x80000000:
                    crc = (crc << 1) ^ 0x04C11DB7 #Polynomial used in STM32
                else:
                    crc = (crc << 1)

        return (crc & 0xFFFFFFFF)

    def _create_cmd_message(self, msg):
        #Encodes a command adding the CRC32
        cmd = list(msg) + list(struct.pack("I", self._crc_stm32(msg)))
        return cmd

    def _encryptMessage(self, data):
        #Encrypts a sequence of bytes using the AES-128 algorithm and
        #adds the CRC-32
        data = AESEncryptor.encrypt("".join(map(chr,data)))
        data32 = struct.unpack("IIII", data)
        data += struct.pack("I", self._crc_stm32(data32))
        return data

    def eraseFLASH(self, nsectors=0xFF):
        #Sends an CMD_ERASE to the bootloader
        self.serial.flushInput()
        self.serial.write(self._create_cmd_message((CMD_ERASE,nsectors)))
        data = self.serial.read(1)
        if len(data) == 1:
            if struct.unpack("b", data)[0] != ACK:
                raise ProgramModeError("Can't erase FLASH")
        else:
            raise TimeoutError("Timeout error")
     
    def getID(self):
        #Sends an CMD_ERASE to the bootloader
        self.serial.flushInput()
        self.serial.write(self._create_cmd_message((CMD_GETID,)))
        data = self.serial.read(1)
        if len(data) == 1:
            if struct.unpack("b", data)[0] != ACK:
                raise ProgramModeError("CMD_GETID failed")
            else:
                data = self.serial.read(2)
                if len(data) == 2:
                    return struct.unpack("h", data)[0]
                raise ProgramModeError("CMD_GETID failed") 
        else:
            raise TimeoutError("Timeout error")

    def writeImage(self, filename):
        #Sends an CMD_WRITE to the bootloader
        #This is method is a generator, that returns its progresses to the caller.
        #In this way, it's possibile for the caller to live-print messages about
        #writing progress
        ih = IntelHex()  
        ih.loadhex(filename)
        yield {"saddr": ih.minaddr(), "eaddr": ih.maxaddr()}

        addr = ih.minaddr()
        content = ih.todict()
        abort = False
        resend = 0
        while addr <= ih.maxaddr():
            if not resend:
                data = []
                saddr = addr
                for i in range(16):
                  try:
                      data.append(content[addr])
                  except KeyError:
                      data.append(0xFF)
                  addr+=1
            try:
                if resend >= 3:
                     abort = True
                     break

                self.serial.flushInput()
                self.serial.write(self._create_cmd_message([CMD_WRITE] + map(ord, struct.pack("I", saddr))))
                ret = self.serial.read(1)
                if len(ret) == 1:
                    if struct.unpack("b", ret)[0] != ACK:
                        raise ProgramModeError("Write abort")
                else:
                    raise TimeoutError("Timeout error")

                encdata = self._encryptMessage(data)
                self.serial.flushInput()
                self.serial.write(encdata)
                ret = self.serial.read(1)
                if len(ret) == 1:
                    if struct.unpack("b", ret)[0] != ACK:
                        raise ProgramModeError("Write abort")
                else:
                    raise TimeoutError("Timeout error")

                yield {"loc": saddr, "resend": resend}
                resend = 0
            except (TimeoutError, ProgramModeError):
                resend +=1

        yield {"success": not abort}


if __name__ == '__main__':
    eraseDone = 0

    def doErase(arg):
        global eraseDone

        flasher.eraseFLASH(0xFF)
        time.sleep(1)
        eraseDone = 1

    flasher = STM32Flasher(sys.argv[1])
    id = flasher.getID()
    print("STM32 MCU Type: ", STM32_TYPE[id])
    print("Erasing Flash memory", end="")
    thread.start_new_thread(doErase, (None,))
    while eraseDone == 0:
        print(".", end="")
        sys.stdout.flush()
        time.sleep(0.5)

    print(" Done")
    print("Loading %s HEX file...." % sys.argv[2])

    for e in flasher.writeImage(sys.argv[2]):
        if "saddr" in e:
            print("\tStart address: ", hex(e["saddr"]))
            print("\tEnd address: ", hex(e["eaddr"]))
        if "loc" in e:
            if e["resend"] > 0:
                end = "\n"
            else:
                end = ""
            print("\r\tWriting address: %s --- %d" % (hex(e["loc"]), e["resend"]), end=end)
            sys.stdout.flush()

        if "success" in e and e["success"]:
            print("\n\tDone")
        elif "success" in e and not e["success"]:
            print("\n\tFailed to upload firmware")