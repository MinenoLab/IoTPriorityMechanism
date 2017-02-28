import binascii
if __name__ == "__main__":
    f=open("DummyBCameraDatas.txt",'w')
    strdata=""
    bytesize=1024*1024*4
    for i in range(bytesize):
    	strdata=strdata+"C"
    f.write(strdata)
