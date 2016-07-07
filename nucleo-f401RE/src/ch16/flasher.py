import sys, serial, binascii, time
from intelhex import IntelHex
from Crypto.Cipher import AES

AES_KEY = "4D:61:73:74:65:72:69:6E:67:20:20:53:54:4D:33:32"

AES_KEY = binascii.unhexlify(AES_KEY.replace(":", ""))
AESEncryptor = AES.new(AES_KEY, AES.MODE_ECB)

ACK  = 0x79
NACK = 0x1F

ser = serial.Serial(sys.argv[1], 38400)
ser.write((0x7f,))
data = ord(ser.read(1))

if data == ACK:
	print("Board is ready to accept commands")

ih = IntelHex()  
ih.loadhex(sys.argv[2])
print(dir(ih))
print hex(ih.minaddr())
print hex(ih.maxaddr())
addr = ih.minaddr()
content = ih.todict()
while addr <= ih.maxaddr():
	data = []
	for i in range(16):
		try:
			# print "Programming: ", hex(addr),
			# print " Content: ", hex(content[addr])
			data.append(content[addr])
		except KeyError:
			data.append(0xFF)
		addr+=1

	print data
	data = AESEncryptor.encrypt("".join(map(chr,data)))
	#data = AESEncryptor.decrypt(data)
	#print map(ord, data)
	ser.write(data)
	time.sleep(0.01)
# content = ih.todict()
# for k in content.keys():
# 	try:
# 		print hex(k), hex(content[k])
# 	except:
# 		pass