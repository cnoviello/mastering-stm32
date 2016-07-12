from __future__ import print_function

import sys, serial, binascii, time, struct, thread
from intelhex import IntelHex
from Crypto.Cipher import AES

AES_KEY = "4D:61:73:74:65:72:69:6E:67:20:20:53:54:4D:33:32"

AES_KEY = binascii.unhexlify(AES_KEY.replace(":", ""))
AESEncryptor = AES.new(AES_KEY, AES.MODE_ECB)

ACK  = 0x79
NACK = 0x1F

CMD_ENTER_PROGRAM_MODE = 0x7f
CMD_ERASE              = 0x43
CMD_GETID              = 0x02
CMD_WRITE              = 0x2b

STM32_TYPE = {
    0x433: "STM32F401RE"
}

class ProgramModeError(Exception):
    pass

class TimeoutError(Exception):
    pass

class STM32Flasher(object):
    def __init__(self, serialPort, baudrate=38400):
        self.serial = serial.Serial(serialPort, baudrate=baudrate, timeout=20)
        self.inProgramMode = False

    def _crc_stm32(self, data):
        crc = 0xFFFFFFFF

        for d in data:
            crc ^= d
            for i in range(32):
                if crc & 0x80000000:
                    crc = (crc << 1) ^ 0x04C11DB7 #Polynomial used in STM32
                else:
                    crc = (crc << 1)

        return (crc & 0xFFFFFFFF)

    def _encryptMessage(self, data):
        print("DAAAA: ", data)
        data = AESEncryptor.encrypt("".join(map(chr,data)))
        data32 = struct.unpack("IIII", data)
        data += struct.pack("I", self._crc_stm32(data32))
        return data

    def enterProgramMode(self):
        self.serial.flushInput()        
        self.serial.write((CMD_ENTER_PROGRAM_MODE,))
        data = self.serial.read(1)
        if len(data) == 1:
            if struct.unpack("b", data)[0] == ACK:
                self.inProgramMode = True
            else:
                raise ProgramModeError("Can't enter in program mode. Try to reset MCU")
        else:
            raise TimeoutError("Timeout error")

    def eraseFLASH(self):
        if not self.inProgramMode:
            raise ProgramModeError("MCU not in program mode")           

        self.serial.flushInput()
        self.serial.write((CMD_ERASE,))
        data = self.serial.read(1)
        if len(data) == 1:
            if struct.unpack("b", data)[0] != ACK:
                raise ProgramModeError("Can't erase FLASH")
        else:
            raise TimeoutError("Timeout error")
     
    def getID(self):
        if not self.inProgramMode:
            raise ProgramModeError("MCU not in program mode")           

        self.serial.flushInput()            
        self.serial.write((CMD_GETID,))
        data = self.serial.read(2)
        if len(data) == 2:
            return struct.unpack("h", data)[0]
        else:
            raise TimeoutError("Timeout error")

    def writeImage(self, filename):
        if not self.inProgramMode:
            raise ProgramModeError("MCU not in program mode")           

        ih = IntelHex()  
        ih.loadhex(filename)
        yield {"saddr": ih.minaddr(), "eaddr": ih.maxaddr()}

        self.serial.flushInput()
        self.serial.write((CMD_WRITE,))
        data = self.serial.read(1)
        if len(data) == 1:
            if struct.unpack("b", data)[0] != ACK:
                print("Received: ", hex(ord(data)))
                raise ProgramModeError("Write abort")
        else:
            raise TimeoutError("Timeout error")


        addr = ih.minaddr()
        content = ih.todict()
        while addr <= ih.maxaddr():
            data = []
            saddr = addr
            for i in range(16):
              try:
                  data.append(content[addr])
              except KeyError:
                  data.append(0xFF)
              addr+=1

            print(map(chr,data))
            data = self._encryptMessage(data)
            print(map(ord,data))
            self.serial.flushInput()
            self.serial.write(data)
            data = self.serial.read(1)
            if len(data) == 1:
                if struct.unpack("b", data)[0] != ACK:
                    raise ProgramModeError("Write abort")
            else:
                raise TimeoutError("Timeout error")

            yield {"loc": saddr}

        data = self._encryptMessage([0]*16)
        self.serial.flushInput()
        self.serial.flushOutput()        
        self.serial.write(data)
        data = self.serial.read(1)
        if len(data) == 1:
            if struct.unpack("b", data)[0] != ACK:
                raise ProgramModeError("Write abort")
        else:
            raise TimeoutError("Timeout error")

if __name__ == '__main__':
    eraseDone = 0

    def doErase(arg):
        global eraseDone

        flasher.eraseFLASH()
        time.sleep(1)
        eraseDone = 1

    flasher = STM32Flasher(sys.argv[1])
    flasher.enterProgramMode()
    id = flasher.getID()
    print("STM32 MCU Type: ", STM32_TYPE[id])
    print("Erasing Flash memory", end="")
    thread.start_new_thread(doErase, (None,))
    while eraseDone == 0:
        print(".", end="")
        sys.stdout.flush()
        time.sleep(0.5)

    # flasher.eraseFLASH()
    print(" Done")
    print("Loading %s HEX file...." % sys.argv[2])

    for e in flasher.writeImage(sys.argv[2]):
        if "saddr" in e:
            print("\tStart address: ", hex(e["saddr"]))
            print("\tEnd address: ", hex(e["eaddr"]))
        if "loc" in e:
            print("\r\tWriting address: ", hex(e["loc"]), end="")
            sys.stdout.flush()

    print("\n\tDone")


# if data == ACK:
#   print("Board is ready to accept commands")

# ih = IntelHex()  
# ih.loadhex(sys.argv[2])
# print(dir(ih))
# print hex(ih.minaddr())
# print hex(ih.maxaddr())
# addr = ih.minaddr()
# content = ih.todict()
# while addr <= ih.maxaddr():
#   data = []
#   for i in range(16):
#       try:
#           # print "Programming: ", hex(addr),
#           # print " Content: ", hex(content[addr])
#           data.append(content[addr])
#       except KeyError:
#           data.append(0xFF)
#       addr+=1

#   print data
#   data = AESEncryptor.encrypt("".join(map(chr,data)))
#   #data = AESEncryptor.decrypt(data)
#   #print map(ord, data)
#   ser.write(data)
#   time.sleep(0.01)
# content = ih.todict()
# for k in content.keys():
#   try:
#       print hex(k), hex(content[k])
#   except:
#       pass