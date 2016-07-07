from Crypto.Cipher import AES
import binascii

msg = "69:c4:e0:d8:6a:7b:04:30:d8:cd:b7:80:70:b4:c5:5a"
# msg = "00:11:22:33:44:55:66:77:88:99:aa:bb:cc:dd:ee:ff"
key = "00:01:02:03:04:05:06:07:08:09:0a:0b:0c:0d:0e:0f"
msg = msg.replace(":", "")
key = key.replace(":", "")
print msg
print len(msg)
data = binascii.unhexlify(msg)
key = binascii.unhexlify(key)
# print key
mode = AES.MODE_ECB
IV = IV = 16 * '\x00' 
encryptor = AES.new(key, mode, IV=IV)
msg2 = encryptor.decrypt(data)
print binascii.hexlify(msg2)

