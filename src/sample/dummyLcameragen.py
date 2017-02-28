import binascii
if __name__ == "__main__":
    f=open("DummyLCameraDatas.txt",'w')
    strdata=""
    bytesize=1024*200
    for i in range(bytesize):
    	strdata=strdata+"L"
    f.write(strdata)
