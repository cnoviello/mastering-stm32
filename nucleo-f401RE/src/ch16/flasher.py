import sys, serial
from intelhex import IntelHex

ACK  = 0x79
NACK = 0x1F

ser = serial.Serial("/dev/tty.usbmodem1443", 38400)
ser.write((0x7f,))
data = ord(ser.read(1))

if data == ACK:
	print("Board is ready to accept commands")

ih = IntelHex()  
ih.loadhex(sys.argv[1])
print(dir(ih))
print hex(ih.minaddr())
print hex(ih.maxaddr())
addr = ih.minaddr()
content = ih.todict()
while addr <= ih.maxaddr():
	if addr not in content.keys():
		print "NON ESISTE: ", hex(addr)
	addr+=1
	# try:
	# 	print "Programming: ", hex(addr),
	# 	print " Content: ", hex(content[addr])
	# 	ser.write((content[addr],))
	# except KeyError:
	# 	ser.write((0,))
	# addr+=1
# content = ih.todict()
# for k in content.keys():
# 	try:
# 		print hex(k), hex(content[k])
# 	except:
# 		pass